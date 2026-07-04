#include "shell.hpp"
#include "process.hpp"
#include <array>
#include <string>
#include <thread>
#include <chrono>
#include <cstdio>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/wait.h>
#endif

using namespace TinyProcessLib;
using namespace std::chrono;

// internal done marker, exit code follows right after it in the same line
constexpr std::array<char, sizeof("__CMD_DONE_SIGNAL__")> DONE_MARKER = {"__CMD_DONE_SIGNAL__"};

shell_t::~shell_t() {
    destroy();
}

bool shell_t::is_valid() {
    return valid_.load();
}

// internal unlocked implementation, called by lock-holding functions to avoid deadlock
bool shell_t::is_running_locked() {
    if (!valid_ || !process_) return false;
    int exit_code{};
    return !process_->try_get_exit_status(exit_code);
}

bool shell_t::is_running() {
    std::lock_guard<std::mutex> lock(mtx_);
    return is_running_locked();
}

bool shell_t::start() {
    std::lock_guard<std::mutex> lock(mtx_);
    if (valid_) return true;

    try {
        // stdout callback: buffer data, filter internal marker, print only user output
        auto stdout_cb = [this](const char* data, size_t size) {
            output_buffer_.append(data, size);
            const size_t marker_len = sizeof(DONE_MARKER) - 1;
            size_t mark_pos = output_buffer_.find(DONE_MARKER.data());

            if (mark_pos == std::string::npos) {
                // no complete marker: print safe part, keep tail bytes to handle split packet
                if (output_buffer_.size() > marker_len - 1) {
                    size_t print_len = output_buffer_.size() - (marker_len - 1);
                    printf("%.*s", static_cast<int>(print_len), output_buffer_.data());
                    fflush(stdout);
                    output_buffer_ = output_buffer_.substr(print_len);
                }
                return;
            }

            // marker found: extract pure user output (everything before marker)
            std::string user_output = output_buffer_.substr(0, mark_pos);
            if (!user_output.empty()) {
                printf("%s", user_output.c_str());
                fflush(stdout);
            }

            // parse exit code from content right after the marker
            const char* code_start = output_buffer_.c_str() + mark_pos + marker_len;
            // skip leading space before exit code
            while (*code_start == ' ') code_start++;
            last_exit_code_ = std::atoi(code_start);

            // preserve remaining content after marker line for next command
            size_t line_end = output_buffer_.find('\n', mark_pos);
            if (line_end != std::string::npos) {
                output_buffer_ = output_buffer_.substr(line_end + 1);
            } else {
                output_buffer_.clear();
            }

            done_.store(true);
        };

        // stderr callback: print directly
        auto stderr_cb = [](const char* data, size_t size) {
            fprintf(stderr, "%.*s", static_cast<int>(size), data);
            fflush(stderr);
        };

#ifdef _WIN32
        // /v:on: enable delayed environment variable expansion for !errorlevel!
        static const std::vector<std::string> args = {
            "cmd.exe", "/q", "/d", "/v:on", "/k", "prompt $"
        };
#else
        static const std::vector<std::string> args = {
            "/bin/bash", "--norc", "-i", "-c", "PS1='$ '; exec bash"
        };
#endif
        process_ = new Process(args, "", stdout_cb, stderr_cb, true);
        valid_.store(true);
        return true;
    } catch (...) {
        valid_.store(false);
        return false;
    }
}

void shell_t::write(const std::string& cmd) {
    std::lock_guard<std::mutex> lock(mtx_);
    if (valid_ && process_) {
        process_->write(cmd + "\n");
    }
}

void shell_t::destroy() {
    std::lock_guard<std::mutex> lock(mtx_);
    if (process_) {
        if (is_running_locked()) {
            process_->kill();
        }
        delete process_;
        process_ = nullptr;
    }
    valid_.store(false);
    output_buffer_.clear();
}

// ------------------------------
// public interfaces
// ------------------------------
shell_t* start_shell() {
    auto* sh = new shell_t();
    if (!sh->start()) {
        delete sh;
        return nullptr;
    }
    return sh;
}

bool run_command(shell_t* sh, const std::string& command) {
    if (!sh || !sh->is_valid() || !sh->is_running()) {
        return false;
    }

    // reset state before each command
    sh->done_.store(false);
    sh->last_exit_code_ = -1;
    sh->output_buffer_.clear();

    // build full command: execute command, then print marker + exit code in SAME line
    // Windows: use !errorlevel! (delayed expansion) to get real exit code of previous command
    std::string full_cmd;
#ifdef _WIN32
    full_cmd = command + " & echo " + std::string(DONE_MARKER.data()) + " !errorlevel!";
#else
    full_cmd = command + "; echo " + std::string(DONE_MARKER.data()) + " $?";
#endif

    sh->write(full_cmd);

    // wait for command completion
    while (true) {
        if (sh->done_.load()) {
            return sh->last_exit_code_ == 0;
        }

        if (!sh->is_running()) {
            sh->destroy();
            return false;
        }

        std::this_thread::sleep_for(milliseconds(1));
    }
}

void kill_shell(shell_t*& sh) {
    if (!sh) return;
    sh->destroy();
    delete sh;
    sh = nullptr;
}
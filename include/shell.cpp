#include "shell.hpp"
#include "process.hpp"
#include <vector>
#include <iostream>
#include <thread>
#include <chrono>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace TinyProcessLib;
using namespace std::chrono;

constexpr char EXIT_CODE_MARK[] = "ERRLEVEL ";

shell_t::~shell_t() {
    destroy();
}

bool shell_t::is_valid() {
    return _valid;
}



bool shell_t::start() {
    std::lock_guard<std::mutex> lock(_mtx);
    if (_valid) return true;

    try {
        auto stdout_cb = [this](const char* data, size_t size) {
            std::string output(data, size);
            if (output.find(EXIT_CODE_MARK) != std::string::npos) {
                try {
                    size_t pos = output.find(EXIT_CODE_MARK);
                    std::string code_str = output.substr(pos + std::strlen(EXIT_CODE_MARK));
                    _exit_code = std::stoi(code_str);
                } catch (...) {}
                return;
            }
            std::cout.write(data, size);
        };

        auto stderr_cb = [](const char* data, size_t size) {
            std::cerr.write(data, size);
        };

#ifdef _WIN32
        static const std::vector<std::string> args = {
            "cmd.exe", "/q", "/d", "/k", "prompt $"
        };
#else
        static const std::vector<std::string> args = {
            "/bin/bash", "--norc", "-i", "-c", "PS1='$ '; exec bash"
        };
#endif
        _process = new Process(args, "", stdout_cb, stderr_cb, true);
        _valid = true;
        return true;
    } catch (...) {
        _valid = false;
        return false;
    }
}

bool shell_t::is_running() {
    std::lock_guard<std::mutex> lock(_mtx);
    if (!_valid || !_process) return false;
    int code{};
    return !_process->try_get_exit_status(code);
}

void shell_t::write(const std::string& cmd) {
    std::lock_guard<std::mutex> lock(_mtx);
    if (_valid && _process) {
        _process->write(cmd + "\n");
    }
}

void shell_t::destroy() {
    std::lock_guard<std::mutex> lock(_mtx);
    if (_process) {
        if (is_running()) _process->kill();
        delete _process;
        _process = nullptr;
    }
    _valid = false;
}

shell_t* start_shell() {
    shell_t* sh = new shell_t();
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

    sh->reset_status();

    std::string full_cmd;
#ifdef _WIN32
    full_cmd = command + " & echo " + EXIT_CODE_MARK + "%errorlevel%";
#else
    full_cmd = command + "; echo " EXIT_CODE_MARK "$?";
#endif

    sh->write(full_cmd);

    while (true) {
        if (sh->_exit_code != -1) {
            const int code = sh->_exit_code;
            if (code == 0) {
                return true;
            } else {
                sh->destroy();
                return false;
            }
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
#pragma once
#include <string>
#include <mutex>
#include <atomic>
#include <functional>

namespace TinyProcessLib {
    class Process;
}

class shell_t {
    friend shell_t* start_shell();
    friend bool run_command(shell_t*, const std::string&);
    friend void kill_shell(shell_t*&);

private:
    TinyProcessLib::Process* process_ = nullptr;
    std::atomic<bool> valid_{false};
    std::atomic<bool> done_{false};
    std::string output_buffer_;   // accumulate output to handle split done marker
    std::mutex mtx_;
    int last_exit_code_ = -1;     // exit code of last executed command

    // disable copy/move semantics to prevent double free
    shell_t(const shell_t&) = delete;
    shell_t& operator=(const shell_t&) = delete;
    shell_t(shell_t&&) = delete;
    shell_t& operator=(shell_t&&) = delete;

    shell_t() = default;

    bool start();
    bool is_running();
    bool is_running_locked();  // internal unlocked version to avoid deadlock
    void write(const std::string& cmd);
    void destroy();

public:
    ~shell_t();
    bool is_valid();
};

shell_t* start_shell();
bool run_command(shell_t* sh, const std::string& command);
void kill_shell(shell_t*& sh);
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
    TinyProcessLib::Process* _process = nullptr;
    std::atomic<bool> _valid{false};
    std::atomic<int>  _exit_code{-1};
    std::mutex _mtx;

    shell_t(const shell_t&) = delete;
    shell_t& operator=(const shell_t&) = delete;
    shell_t() = default;

    bool start();
    bool is_running();
    void write(const std::string& cmd);  // 修复：统一返回值为void
    void destroy();
    void reset_status(){
        this->_exit_code = -1;
    }  // 修复：正确声明函数

public:
    ~shell_t();
    bool is_valid();
};

shell_t* start_shell();
bool run_command(shell_t* sh, const std::string& command);
void kill_shell(shell_t*& sh);
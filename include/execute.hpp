#pragma once

#include "main.h"
#include "root.hpp"
#include "shell.hpp"
#include <string>
#include <memory>
#include <vector>

typedef struct exec_args {
    const std::vector<std::string> task;
    const std::string target;
    int depth = 0;
    bool force_legacy = false;
} exec_t;

namespace maker {
    class executor_t {
    private:
        shell_t* shell_ptr_ = nullptr;
        bool is_legacy_ = false;
        bool is_valid_ = true;

        bool legacy_run(const std::string& command);
        bool new_run(const std::string& command);

#ifdef _WIN32
        inline static auto sys_popen = _popen;
        inline static auto sys_pclose = _pclose;
#else
        inline static auto sys_popen = popen;
        inline static auto sys_pclose = pclose;
#endif

    public:
        explicit executor_t(bool force_legacy = false);
        ~executor_t();

        // disable copy/move to avoid double free of shell resource
        executor_t(const executor_t&) = delete;
        executor_t& operator=(const executor_t&) = delete;
        executor_t(executor_t&&) = delete;
        executor_t& operator=(executor_t&&) = delete;

        bool execute_command(const std::string& command);
    };
}

// global execution interfaces
int execute_command(const std::string& command);
int execute(const exec_t& args);
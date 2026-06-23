#include "execute.hpp"
#include "get.h"
#include "print.h"
#include "shell.hpp"
#include <array>
#include <cstdio>
#include <string>

#ifdef _WIN32
#else
#include <sys/wait.h>
#endif


// ------------------------------
// global command execution via popen
// ------------------------------
int execute_command(const std::string& command) {
#ifdef _WIN32
    FILE* pipe = _popen(command.c_str(), "r");
#else
    FILE* pipe = popen(command.c_str(), "r");
#endif
    if (!pipe) {
        print_status(1);
        printf("System problem.\n");
        printf("Try to popen. Failed. Stop.\n");
        return -1;
    }

    try {
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            printf("111%s", buffer);
        }
    } catch (...) {
#ifdef _WIN32
        _pclose(pipe);
#else
        pclose(pipe);
#endif
        print_status(1);
        printf("System problem.\n");
        printf("Error during command execution. Stop.\n");
        return -2;
    }

    // extract real exit code (Linux requires WEXITSTATUS macro)
#ifdef _WIN32
    int return_code = _pclose(pipe);
#else
    int status = pclose(pipe);
    if (status == -1) {
        print_status(1);
        printf("System problem.\n");
        printf("Try to pclose. Failed. Stop.\n");
        return -3;
    }
    int return_code = WEXITSTATUS(status);
#endif

    return return_code;
}

// ------------------------------
// maker::executor_t implementation
// ------------------------------
namespace maker {

executor_t::executor_t(bool force_legacy) {
    if (force_legacy) {
        is_legacy_ = true;
        return;
    }

    shell_ptr_ = start_shell();
    if (!shell_ptr_) {
        is_legacy_ = true;
        print_status(2);
        printf("Shell start failed. Fallback to legacy mode.\n");
    }
}

executor_t::~executor_t() {
    if (shell_ptr_) {
        kill_shell(shell_ptr_);
        shell_ptr_ = nullptr;
    }
}

bool executor_t::legacy_run(const std::string& command) {
    if (!is_valid_) {
        return false;
    }

    FILE* pipe = sys_popen(command.c_str(), "r");
    if (!pipe) {
        print_status(1);
        printf("Try to popen. Failed. Stop.\n");
        is_valid_ = false;
        return false;
    }

    try {
        std::array<char, 128> buffer;
        while (fgets(buffer.data(), sizeof(buffer), pipe) != nullptr) {
            printf("%s", buffer.data());
        }
    } catch (...) {
        sys_pclose(pipe);
        print_status(1);
        printf("Error during command execution. Stop.\n");
        return false;
    }

    int return_code = sys_pclose(pipe);
#ifndef _WIN32
    if (return_code != -1) {
        return_code = WEXITSTATUS(return_code);
    }
#endif

    if (return_code == -1) {
        print_status(1);
        printf("Try to pclose. Failed. Stop.\n");
        return false;
    }

    return return_code == 0;
}

bool executor_t::new_run(const std::string& command) {
    if (!is_valid_) {
        return false;
    }
    return run_command(shell_ptr_, command);
}

bool executor_t::execute_command(const std::string& command) {
    if (is_legacy_) {
        return legacy_run(command);
    }
    return new_run(command);
}

} // namespace maker

// ------------------------------
// task execution entry
// ------------------------------
int execute(const exec_t& args) {
    if (args.depth > 30) {
        print_status(1);
        printf("Too deep recursion. Stop.\n");
        return -4;
    }

    auto executor = std::make_unique<maker::executor_t>(args.force_legacy);

    for (const auto& item : args.task) {
        bool is_direct_cmd = command_parser(item);
        if (is_direct_cmd && !args.target.empty()) {
            // execute command directly
            print_status(3);
            printf("Executing command: %s\n",item.c_str());
            bool success = executor->execute_command(item);
            if (!success) {
                print_status(1);
                printf("Command execute failed. Stop.\n");
                return 1;
            }
        } else {
            // execute subtask
            constexpr size_t SUBTASK_PREFIX_LEN = 5;
            if (item.size() < SUBTASK_PREFIX_LEN) {
                print_status(1);
                printf("Invalid subtask format: %s. Stop.\n", item.c_str());
                return 1;
            }

            std::string sub_task_name = item.substr(SUBTASK_PREFIX_LEN);
            print_status(3);
            printf("Executing subtask: %s\n", sub_task_name.c_str());

            exec_t sub_args = {
                .task=get_task(sub_task_name),
                .target=sub_task_name,
                .depth=args.depth + 1,
                .force_legacy=args.force_legacy
            };

            int res = execute(sub_args);
            if (res != 0) {
                return res;
            }
        }
    }

    return 0;
}
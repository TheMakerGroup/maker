#include "execute.hpp"
#include "get.hpp"
#include "print.hpp"
#include "shell.hpp"
#include <array>
#include <cstdio>
#include <stdexcept>
#include <string>

#ifdef _WIN32
#else
#include <sys/wait.h>
#endif

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

int execute(exec_t& args) {

    int depth = 0;

    auto executor = std::make_unique<maker::executor_t>(args.force_legacy);

    while(!args.list.empty()){

        if (depth > 30) {
            print_status(1);
            printf("Too deep recursion. Stop.\n");
            return 1;
        }
        
        std::string cmd = args.list.front();
        args.list.pop_front();

        bool is_direct_cmd = maker::get::command_parser(cmd);
        if(is_direct_cmd){
            // execute command directly
            print_status(3);
            printf("Executing command: %s\n",cmd.c_str());
            bool success = executor->execute_command(cmd);
            if (!success) {
                print_status(1);
                printf("Executing command: '%s' failed. Stop.\n", cmd.c_str());
                return 1;
            }
        }else{
            // execute subtask
            constexpr size_t SUBTASK_PREFIX_LEN = 6;
            if (cmd.size() < SUBTASK_PREFIX_LEN) {
                print_status(1);
                printf("Invalid subtask format: %s. Stop.\n", cmd.c_str());
                return 1;
            }

            std::string sub_task_name = cmd.substr(SUBTASK_PREFIX_LEN);
            print_status(3);
            printf("Explanding subtask: %s\n", sub_task_name.c_str());

            std::deque<std::string> sub_list;

            try{
                sub_list = maker::get::get_task(sub_task_name);
            }catch(const std::runtime_error& e){
                print_status(1);
                printf("%s", e.what());
                return 1;
            }

            args.list.insert(args.list.begin(),sub_list.begin(),sub_list.end());
            depth++;
        }
    }

    return 0;
}
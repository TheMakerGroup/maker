#include "main.h"
#include "print.h"
#include "shell.hpp"

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
        // read output
        char buffer[128];
        std::string result;
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            std::cout << buffer;  // print result
            result += buffer;
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
 
    // get status
#ifdef _WIN32
    int return_code = _pclose(pipe);
#else
    int return_code = pclose(pipe);
#endif
    if (return_code == -1) {
        print_status(1);
        printf("System problem.\n");
        printf("Try to pclose. Failed. Stop.\n");
        return -3;
    }

    if (return_code == 0) {
        return 0;
    }
    else {
        
        return return_code;
    }
}

int execute(const exec_t& args) {
    if (args.depth > 30){
        print_status(1);
        printf("Too deep recursion. Stop.\n");
        return -4;
    }
    bool is_legacy = false;
    shell_t* shell = nullptr;
    if(!args.force_legacy){
        shell = start_shell();
        if(!shell){
            is_legacy = true;
            print_status(2);
            printf("Shell start failed. Fallback to legacy mode.\n");
        }
    }else{
        is_legacy = true;
    }

    for (const auto & i : args.task) {
        if(bool pass = command_paser(i); pass && !args.target.empty()){
			/* Direct command execute */
            print_status(3);
            printf("Executing command: %s\n", i.c_str());
            int res = 0;
            if(is_legacy){
                res = execute_command(i); 
            }else{
                res = run_command(shell, i);
            }
            if (res != 0){
                //print_code_indicator(file_name, i, 1);
                print_status(1);
                printf("Command execute error with code %d. Stop.\n", res);
                return 1;
            }
        }
        else{
            /* Sub-task execute */
            std::string sub_task_name = i.substr(5);
            print_status(3);
            printf("Executing sub-task: %s\n", sub_task_name.c_str());
            exec_t arg_sub = {get_task(sub_task_name),
                            sub_task_name,
                            args.depth + 1,
                            args.force_legacy};
            if (const int res = execute(arg_sub); res != 0){
                return res;
            }
        }
    }
    return 0; //default return
}
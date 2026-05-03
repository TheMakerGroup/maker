#include "main.h"
#include <uv.h>

static shell_t* pub_shell = nullptr;

int execute_command(const std::string& command) {
    print_status(3);
    printf("Executing command: %s\n", command.c_str());
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

int execute(exec args) {
    if (args.depth > 30){
        print_status(1);
        printf("Too deep recursion. Stop.\n");
        return -4;
    }
    bool is_lagacy = true;

    uv_loop_t* loop = uv_default_loop();
    shell_t* shell = start_shell(loop);
    if (!shell) {
        print_status(2);
        printf("Shell start failed. Fallback to lagacy mode.\n");
        is_lagacy = true;
    }
    else {
        // prepare for process exit
        pub_shell = shell;
        uv_signal_t sigint;
        uv_signal_init(uv_default_loop(), &sigint);
        uv_signal_start(&sigint, on_sigint, SIGINT);
    }

    for (const auto & i : args.task) {
        bool pass = command_paser(i);
        if(pass && !args.target.empty()){
			/* Direct command execute */
            int res = 0;
            if (is_lagacy) {
                res = execute_command(i);
            }
            else {
                res = shell_exec(shell, i.c_str());
            }
            if (res != 0){
                if (!is_lagacy) {
                    kill_shell(shell);
                }
                print_code_indicator(args.file_name, i, 1);
                printf("Command execute error. Stop.\n");
                return 1;
            }
        }
        else{
            /* Sub-task execute */
            print_status(3);
            printf("Executing sub-task: %s\n", args.target.c_str());
            std::vector<std::string> sub_task = get_task(args.target, args.file_name);
            exec sub = { sub_task, args.target, args.file_name };
            const int res = execute(sub);
            if (res != 0){
                return res;
            }
        }
    }
    return 0; //default return
}
void on_sigint(uv_signal_t* handle, int signum) {
    (void)signum;

    uv_signal_stop(handle);
    uv_close((uv_handle_t*)handle, NULL);

    kill_shell(pub_shell);

    uv_stop(uv_default_loop());

    pub_shell = nullptr;
}
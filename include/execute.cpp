#include "main.h"

auto execute_command(const std::string& command) -> int {
    print_status(3);
    printf("Executing command: %s\n", command.c_str());
#ifdef _WIN32
    FILE* pipe = _popen(command.c_str(), "r");
#else
    FILE* pipe = popen(command.c_str(), "r");
#endif
    if (pipe == nullptr) {
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
     
    return return_code;
   
}

auto execute(const std::vector<std::string>& task, const std::string& target, std::string file_name, const int depth) -> int {
    if (depth > 30){
        print_status(1);
        printf("Too deep recursion. Stop.\n");
        return -4;
    }
    for (const auto & i : task) {
        if(bool pass = command_paser(i); pass && !target.empty()){
			/* Direct command execute */
            if (const int res = execute_command(i); res != 0){
                print_code_indicator(file_name, i, 1);
                printf("Command execute error with code %d. Stop.\n", res);
                return 1;
            }
        }
        else{
            /* Sub-task execute */
            print_status(3);
            printf("Executing sub-task: %s\n", target.c_str());
            std::vector<std::string> sub_task = get_task(target, file_name);
            if (const int res = execute(sub_task, target, file_name, depth + 1); res != 0){
                return res;
            }
        }
    }
    return 0; //default return
}
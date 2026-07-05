/* Maker Gueder798
This is under GNU GPLv3+ license.
The file has the main function.*/

/*  If you cannot find a function, you can see
include directory for more details.

Note: 
1.If one function is used to get, look for get.cpp.
2.If one function is used to execute, look for execute.cpp.
3.If one function is used to show information, look for print.cpp.
*/


#include "include/get.h"
#include "include/print.h"
#include "include/execute.hpp"
#include <stdexcept>
#ifdef _WIN32
#include <windows.h>
#endif

int main(const int argc, char** argv) {

#ifdef _WIN32 //Enable ANSI Output
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hOut, &mode);
    SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif


    //const std::string target = parse_arg(argc,argv,status);
    const arg_t result = parse_arguments(argc, argv);

    if(result.is_err){
        return 1;
    }

    if(result.should_exit){
        return 0;
    }

	if (result.make_target.empty()) {
		return 1;
	}

    std::deque<std::string> list;
    try{
        list = get_task(result.make_target);
    }catch(std::runtime_error& e){
        print_status(1);
        printf("%s", e.what());
        return 1;
    }

    if (list.empty()) {
        print_status(1);
        printf("Unknown task: %s\n", result.make_target.c_str());
        return -1;
    }

    exec_t args = {.list=list, 
            .target=result.make_target,
            .depth=0,
            .force_legacy=result.force_legacy};

    if (const int res = execute(args); res == 0) {
		print_status(3);
		printf("All tasks executed successfully.\n");
	}else{
        return 1;
    }
	return 0;
}
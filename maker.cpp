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
#include "include/main.h"

int main(const int argc, char** argv) {

#ifdef _WIN32 //Enable ANSI Output
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hOut, &mode);
    SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif

    std::string file_name;
	int status = 0;

    arg_t result = parse_arguments(argc, argv);

    if(result.should_exit){
        return 1;
    }

	if (result.make_target.empty() && status == 0) {
		return 0;
	}else if (result.make_target.empty() && status == 1) {
		return 1;
	}

    const std::vector<std::string> list = get_task(result.make_target,file_name);

    if(!list.empty() && list[0].empty()){
        return 1;
    }
    if (list.empty()) {
        print_status(0);
        printf("Unknown task: %s\n", result.make_target.c_str());
        return 2;
    }
    exec arg = { list, result.make_target, file_name };
    const int res = execute(arg);
    if ( res == 0) {
		print_status(3);
		printf("All tasks executed successfully.\n");
	}
	else if(res == 1){
		return 1;
	}
	return 0;
}
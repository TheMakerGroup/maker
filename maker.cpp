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


#include "include/main.h"

int main(const int argc, char** argv) {

#ifdef _WIN32 //Enable ANSI Output
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hOut, &mode);
    SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif

    std::string file_name;
	int status;

    const std::string target = parser_arg(argc,argv,status);

	if (target.empty() && status == 0) {
		return 0;
	}else if (target.empty() && status == 1) {
		return -1;
	}

    const std::vector<std::string> list = get_task(target,file_name);

    if (list.empty()) {
        print_status(1);
        printf("Unknown task: %s", target.c_str());
        return -1;
    }
	int res = execute(list, target,file_name);

	if (res == 0) {
		print_status(3);
		printf("All tasks executed successfully.\n");
	}
	else if(res == 1){
		return -1;
	}
	return 0;
}
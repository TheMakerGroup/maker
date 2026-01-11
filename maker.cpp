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

#ifdef DEBUG // Check if is debug mode
	info_out(2); //Fix problem here
	printf("You are using debug release.\n"); // Give a warning
#endif

	std::string target;

    std::string file_name;

	//Colorful print will come out soon
    switch (argc) {
    case 1:
        command_print(1,get_command(argc,argv));
        printf("No action input. Stop.\n");
        return -1;

    case 2:
        if (strcmp(argv[1], "make") == 0) {
            command_print(2,get_command(argc,argv));
            printf("No task input. Using default task.\n");
            target = "default";
            break;
        }
        else if (strcmp(argv[1], "-h") == 0) {
            usage();
            return 0;
        }
        else if (strcmp(argv[1], "-v") == 0) {
            about();
            return 0;
        }
        else {
            command_print(1, get_command(argc,argv));
            printf("Invalid argument: %s\n", argv[1]);
            return -1;
        }

    case 3:
        if (strcmp(argv[1], "make") == 0) {
            target = argv[2];
            break;
        }
        else if (strcmp(argv[1], "-h") == 0) {
            usage();
            return 0;
        }
        else if (strcmp(argv[1], "-v") == 0) {
            about();
            return 0;
        }
        else {
            command_print(1,get_command(argc,argv));
            printf("Invalid argument: %s\n", argv[1]);
            return -1;
        }

    default:
        command_print(1, *argv);
        printf("Too many arguments. Stop.\n");
        return -1;
    }

    const std::vector<std::string> list = get_task(target,file_name);

    if (list.empty()) {
        command_print(1,get_command(argc,argv));
        printf("Unknown task: %s", target.c_str());
        return -1;
    }
	int res = execute(list, target,file_name);

	if (res == 0) {
		info_out(1);
		printf("All tasks executed successfully.\n");
	}
	else if(res == 1){
		return -1;
	}
	return 0;
}
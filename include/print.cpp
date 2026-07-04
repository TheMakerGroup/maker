#include <string>
#include <iostream>

const static std::string ver = "1.3";

void about() {
    printf("maker %s ",ver.c_str());
std::cout << R"(Copyright (C) 2026  Gueder
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it
under certain conditions; 
License GPLv3+: GNU GPL version 3 or later 
<http://gnu.org/licenses/gpl.html> for details.
)";
}

void usage(){
    std::cout << R"(          
Usage: maker <action> [task] [options]

Actions:
    make         Make the project following the task in maker.yaml.
    help         Show this help.
    version      Show version info.
Option(s):
    --force-legacy      Using legacy mode to execute command.

Note:
    Using --force-legacy option to action help or version is invalid.
Nothing will happen.
)";
}

inline void print_colored(const std::string& str, const int color) {
	// Attention: Some systems may not support ANSI escape codes.
    std::cout << "\033[38;5;" << color << "m" << str;
    // Reset to default color
    std::cout << "\033[0m";
}

//inline void colored_out_extra(const std::string& str) {
    //Same in colored_out
//    std::cout << "\033[1;37m" << str;
//    std::cout << "\033[0m";
//}

/* This function includes the new style of maker.
 * Type 1: error.
 * Type 2: warning.
 * Type 3: info.
 */
void print_status(const int type) {
    std::string category;
    if (type == 1) {
        category = "\033[1;31merror\033[0m";
    } else if (type == 2) {
        category = "\033[1;38;5;208mwarning\033[0m";
    } else if (type == 3) {
        category = "info";
    }

    std::cout << category << ":" <<'\n';
}
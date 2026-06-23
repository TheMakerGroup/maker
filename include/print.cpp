#include <string>
#include <iostream>
#include <vector>
#include <fstream>

const static std::string ver = "1.2";

void about() {
    printf("maker %s ",ver.c_str());
std::cout << R"(Copyright (C) 2025  Gueder
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it
under certain conditions; 
License GPLv3+: GNU GPL version 3 or later 
<http://gnu.org/licenses/gpl.html> for details.
)";
}

void usage(){
    std::cout << R"(          
Usage: maker [actions] [options] [rule]

Tips: 
1. If the actions is make, config file DO NOT define default rule,
the rule is necessary.

Actions:
    make         Make the project following the rule in maker.yml.

Options:
    -h           Show help.
    -v           Show version.
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
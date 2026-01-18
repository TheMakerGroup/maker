#include "main.h"

std::string ver = "1.1";

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

inline int get_visual_column(const std::string& utf8_str, const size_t byte_pos) {
    int column = 1;
    size_t i = 0;
    while (i < byte_pos && i < utf8_str.size()) {
        auto c = static_cast<unsigned char>(utf8_str[i]);
        if (c < 0x80) {
            column++; i++;
        } else if ((c & 0xE0) == 0xC0) {
            column++; i += 2;
        } else if ((c & 0xF0) == 0xE0) {
            column++; i += 3;
        } else if ((c & 0xF8) == 0xF0) {
            column++; i += 4;
        } else {
            column++; i++;
        }
    }
    return column;
}

bool read_utf8_file(const std::string& filename, std::vector<std::string>& lines) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    const std::vector<char> file_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    size_t content_start = 0;
    if (file_content.size() >= 3) {
        auto bom1 = static_cast<unsigned char>(file_content[0]);
        auto bom2 = static_cast<unsigned char>(file_content[1]);
        auto bom3 = static_cast<unsigned char>(file_content[2]);
        if (bom1 == 0xEF && bom2 == 0xBB && bom3 == 0xBF) {
            content_start = 3;
        }
    }

    std::string utf8_content(file_content.data() + content_start, file_content.size() - content_start);
    std::string normalized_content;
    for (size_t i = 0; i < utf8_content.size(); i++) {
        if (utf8_content[i] == '\r') {
            if (i+1 < utf8_content.size() && utf8_content[i+1] == '\n') {
                normalized_content += '\n'; i++;
            } else {
                normalized_content += '\n';
            }
        } else {
            normalized_content += utf8_content[i];
        }
    }

    size_t pos = 0;
    while (pos < normalized_content.size()) {
        const size_t newline_pos = normalized_content.find('\n', pos);
        if (newline_pos == std::string::npos) {
            lines.push_back(normalized_content.substr(pos));
            break;
        }
        lines.push_back(normalized_content.substr(pos, newline_pos - pos));
        pos = newline_pos + 1;
    }

    return true;
}

void print_code_indicator(const std::string& filename, const std::string& target_str, int msg_type) {
    std::string msg_label;
    if (msg_type == 1) {
        msg_label = "\033[1;31merror\033[0m"; //Red
    } else if (msg_type == 2) {
        msg_label = "\033[1;38;5;208mwarning\033[0m"; //Orange
    } else {
        throw std::invalid_argument("");
    }

    std::vector<std::string> lines;
    if (!read_utf8_file(filename, lines)) {
        return;
    }

    int target_line_num = -1;
    size_t target_byte_pos = std::string::npos;
    std::string target_line;

    for (int line_num = 1; line_num <= static_cast<int>(lines.size()); line_num++) {
        const std::string& line = lines[line_num - 1];
        const size_t str_start_idx = line.find(target_str);
        if (str_start_idx == std::string::npos) continue;

        if (str_start_idx > 0 && line[str_start_idx - 1] == ' ') {
            target_line_num = line_num;
            target_byte_pos = str_start_idx;
            target_line = line;
            break;
        }
    }

    if (target_line_num == -1) {
        return;
    }

    const int target_col_num = get_visual_column(target_line, target_byte_pos);

    const int length = target_str.length() - 1;
    std::string code_err;

    for (int i = 0; i < length; i++)
    {
        code_err.push_back('~');
    }

    std::string first_line = "\033[1m" + filename + ":" + std::to_string(target_line_num) + ":" +
                             std::to_string(target_col_num) + ": " + "\033[0m" + msg_label + ": ";
    std::string second_line = "    " + std::to_string(target_line_num) + " | " + target_line;

    std::string line_num_str = std::to_string(target_line_num);
    int prefix_len = 4 + line_num_str.length() + 3;
    std::string third_line_prefix = std::string(prefix_len - 2, ' ') + "|";
    int spaces_before_caret = target_col_num - 1;
    std::string third_line = third_line_prefix + std::string(spaces_before_caret, ' ') + " ^" + code_err;

    std::cout << first_line << std::endl;
    std::cout << second_line << std::endl;
    std::cout << third_line << std::endl;
}

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

    std::cout << category << ":" <<std::endl;
}
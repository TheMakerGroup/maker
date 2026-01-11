#pragma once

#include "main.h"

void usage();
void about();
void colored_out(const std::string& str, int color);
void info_out(int tape);
//void colored_out_extra(const std::string& str);
void print_code_indicator(const std::string& filename, const std::string& target_str, int msg_type);
void command_print(int type,const std::string& cmd);
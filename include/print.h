#pragma once

#include "main.h"

void usage();
void about();
void print_colored(const std::string& str, int color);
void print_info(int tape);
//void colored_out_extra(const std::string& str);
void print_code_indicator(const std::string& filename, const std::string& target_str, int msg_type);
void print_status(int type);
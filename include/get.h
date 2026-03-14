#pragma once

#include "main.h"

YAML::Node yml_paser(std::string& file_name);
bool command_paser(const std::string& command);
std::vector<std::string> get_task(const std::string& target, std::string& file_name);
std::string get_command(int argc, char**& argv);
std::string parse_arg(int argc, char** argv,int& status);
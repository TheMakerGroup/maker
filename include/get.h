#pragma once

#include "main.h"

YAML::Node yml_paser(std::string& file_name);
std::string command_paser(const std::string& command);
std::string cleaner(const std::string& input);
std::vector<std::string> get_task(const std::string& target, std::string& file_name);
std::string get_command(const int argc, char**& argv);
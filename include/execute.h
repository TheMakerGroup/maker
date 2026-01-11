#pragma once

#include "main.h"

int execute_command(const std::string& command);
int execute(const std::vector<std::string>& task, const std::string& target, std::string file_name, int depth = 0);
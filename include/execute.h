#pragma once

#include "main.h"

typedef struct exec{
    const std::vector<std::string> task;
    const std::string target;
    int depth = 0;
    bool force_legacy = false;
} exec_t;

int execute_command(const std::string& command);
int execute(const exec_t& args);
#pragma once

#include "main.h"
#include <uv.h>

struct exec{
    const std::vector<std::string>& task;
    const std::string& target;
    std::string file_name;
    int depth = 0;
};

int execute_command(const std::string& command);
int execute(exec args);
void on_sigint(uv_signal_t* handle, int signum);
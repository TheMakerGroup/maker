#pragma once

#include "main.h"
#include <unordered_map>

enum class cmd {
    NO_COMMAND,
    MAKE,
    HELP,
    VERSION
};

typedef struct parse_result {
    std::string make_target;
    int exit_code;
    bool should_exit;
    cmd command;
} arg_t;

static const std::unordered_map<std::string, cmd> g_command_map = {
    {"make",  cmd::MAKE},
    {"-h",    cmd::HELP},
    {"-v",    cmd::VERSION}
};

YAML::Node yml_paser(std::string& file_name);
bool command_paser(const std::string& command);
std::vector<std::string> get_task(const std::string& target, std::string& file_name);
std::string get_command(int argc, char**& argv);
arg_t parse_arguments(const int argc, char** argv);
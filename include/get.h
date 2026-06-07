#pragma once

#include "main.h"

enum class cmd {
    NO_COMMAND,
    MAKE,
    HELP,
    VERSION,
    FORCE_LEGACY
};

typedef struct parse_result {
    std::string make_target;
    int exit_code;
    bool should_exit;
    cmd command;
    bool force_legacy;
} arg_t;

static const std::unordered_map<std::string, cmd> g_command_map = {
    {"make", cmd::MAKE},
    {"-h", cmd::HELP},
    {"-v", cmd::VERSION},
    {"--force-legacy", cmd::FORCE_LEGACY}
};


YAML::Node yml_paser(std::string& file_name);
bool command_paser(const std::string& command);
std::vector<std::string> get_task(const std::string& target);
std::string get_command(int argc, char**& argv);
std::string parse_arg(int argc, char** argv,int& status);
arg_t parse_arguments(const int argc, char** argv);
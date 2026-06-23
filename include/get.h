#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

#include <yaml-cpp/yaml.h>

enum class cmd : uint8_t {
    NO_COMMAND,
    MAKE,
    HELP,
    VERSION,
    FORCE_LEGACY
};

using arg_t = struct parse_result {
    std::string make_target;
    int exit_code;
    bool should_exit;
    bool is_err;
    cmd command;
    bool force_legacy;
};

static const std::unordered_map<std::string, cmd> g_command_map = {
    {"make", cmd::MAKE},
    {"help", cmd::HELP},
    {"version", cmd::VERSION},
    {"--force-legacy", cmd::FORCE_LEGACY}
};


YAML::Node yml_paser(std::string& file_name);
bool command_parser(const std::string& command);
std::vector<std::string> get_task(const std::string& target);
std::string get_command(int argc, char**& argv);
std::string parse_arg(int argc, char** argv,int& status);
arg_t parse_arguments(const int argc, char** argv);
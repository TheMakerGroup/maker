#pragma once

#include <cstdint>
#include <deque>
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
    cmd command;
    bool force_legacy;
};

static const std::unordered_map<std::string, cmd> g_command_map = {
    {"make", cmd::MAKE},
    {"help", cmd::HELP},
    {"version", cmd::VERSION},
    {"--force-legacy", cmd::FORCE_LEGACY}
};


void yml_paser(); // jump to maker::get::set_yaml_node()
bool judge_command(const std::string& command);
std::deque<std::string> get_task_new(const std::string& target);
std::string get_command(int argc, char**& argv);
std::string parse_arg(int argc, char** argv,int& status);
arg_t parse_arguments(const int argc, char* argv[]);
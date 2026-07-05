#include "get.hpp"
#include "print.hpp"

#include <cstdlib>
#include <deque>
#include <stdexcept>
#include <string>
#include <yaml-cpp/yaml.h>

constexpr std::string config_file_name = "maker.yaml";
namespace maker::get {
void yml_paser(const std::string& file_name) {

    YAML::Node config;

    try {
        config = YAML::LoadFile(file_name);
    }catch (const YAML::BadFile&) {
        throw std::runtime_error("Can't load configuration file. Stop.\n");
    }

    if (config.IsNull()) {
        throw std::runtime_error("Empty configuration file. Stop.\n");
    }

    YAML::Node tasks = config["tasks"];
    if (!tasks) {
        throw std::runtime_error("'tasks' key not found in configuration file. Stop.\n");
    }

    // Check if default in tasks
    if (!tasks["default"]) {
        print_status(2);
        printf("No 'default' task found. Continuing execution.\n");
    }
    root = tasks;
}

bool command_parser(const std::string& command) {
    if (command.find("tasks.") != std::string::npos) {
        return false;
    }
    return true;
}

std::deque<std::string> get_task(const std::string& target) {
    
    if (!root) {
        return {};
    }
    std::deque<std::string> task;

    if (!root.IsMap()){
        throw std::runtime_error("Invalid tasks format in configuration file. Stop.\n");
    }
    if (root[target].IsNull()) {
        throw std::runtime_error("Unknown task. Stop.\n");
    }
    
    YAML::Node list = root[target];

    for (size_t i = 0; i < list.size(); i++) {
        task.push_back(list[i].as<std::string>());
    }
    return task;
}

std::string get_command(const int argc, char**& argv) {
    std::string command;
    for (int i = 0; i < argc; i++) {
        command.append(argv[i]);
        command.append(" ");
    }
    return command;
}

arg_t parse_arguments(const int argc, char** argv) {
    arg_t result{};
    // Initialize all fields with explicit default values
    result.make_target = "";
    result.should_exit = false;
    result.command = cmd::NO_COMMAND;
    result.force_legacy = false;

    // Handle scenario with no user input arguments
    if (argc == 1) {
        print_status(1);
        printf("No action input. Stop.\n");
        result.should_exit = true;
        result.is_err = true;
        return result;
    }

    int i = 1;
    // Traverse all input arguments, index i controls parameter consumption progress
    while (i < argc) {
        const std::string current_arg = argv[i];
        auto cmd_iterator = g_command_map.find(current_arg);

        // Unknown argument detected: report error and return immediately
        if (cmd_iterator == g_command_map.end()) {
            print_status(1);
            printf("Invalid argument: %s\n", current_arg.c_str());
            result.should_exit = true;
            result.is_err = true;
            return result;
        }

        const cmd current_cmd = cmd_iterator->second;
        switch (current_cmd) {
            // Immediate-exit commands: highest priority, execute and return directly
            case cmd::HELP:
                usage();
                result.should_exit = true;
                return result;

            case cmd::VERSION:
                about();
                result.should_exit = true;
                return result;

            // Flag-type option: no follow-up parameter, set flag and increment index
            case cmd::FORCE_LEGACY:
                result.force_legacy = true;
                print_status(2);
                printf("Force using legacy mode.\n");
                i++;
                break;

            // Argument-bearing command: consumes next token as its parameter
            case cmd::MAKE:
                result.command = cmd::MAKE;
                i++; // Move index to next position to check for target parameter

                if (i < argc) {
                    const std::string next_arg = argv[i];
                    // If next token is not a registered command, treat it as make target
                    if (g_command_map.find(next_arg) == g_command_map.end()) {
                        result.make_target = next_arg;
                        i++; // Consume the target parameter, move index forward
                    } else {
                        // Next token is another command/option, use default target
                        // Do not increment i, leave the token for next iteration
                        print_status(2);
                        printf("No task input. Using default task.\n");
                        result.make_target = "default";
                    }
                } else {
                    // Reached end of argument list, use default target
                    print_status(2);
                    printf("No task input. Using default task.\n");
                    result.make_target = "default";
                }
                break;

            default:
                i++;
                break;
        }
    }

    // Post-validation: ensure at least one valid main command is provided
    if (result.command == cmd::NO_COMMAND) {
        print_status(1);
        printf("No valid command specified. Stop.\n");
        result.should_exit = true;
    }

    return result;
}
}
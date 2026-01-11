#include "main.h"

YAML::Node yml_paser(std::string& file_name) {
#ifdef DEBUG // Note: test.yml won't be checked in the future.
    const std::vector<std::string> files = { "maker.yml", "Maker.yml", "maker.yaml", "Maker.yaml", "test.yml" };
#else
    const std::vector<std::string> files = { "maker.yml", "Maker.yml", "maker.yaml", "Maker.yaml" };
#endif
    YAML::Node config;

    for (const auto& filename : files) {
        try {
            file_name = filename;
            config = YAML::LoadFile(filename);
            break;
        }
        catch (const YAML::BadFile&) {}
    }

    if (!config) {
        info_out(3);
        printf("Configuration file not found. Stop.\n");
        return {};
    }

    YAML::Node tasks = config["tasks"];
    if (!tasks) {
        info_out(3);
        printf("'tasks' key not found in configuration file. Stop.\n");
        return {};
    }

    // Check if default in tasks
    if (!tasks["default"]) {
        info_out(2);
        printf("No 'default' task found. Continuing execution.\n");
    }

    return tasks;
}

std::string command_paser(const std::string& command) {
    info_out(1);
    printf("Parsing command: %s\n", command.c_str());
    if (command.find("tasks.") != std::string::npos) {
        std::string target = command.substr(6, command.length());
        return target;
    }
    return "123";
}


std::string cleaner(const std::string& input) {
    // If a newline is found, return the substring up to that position
    if (const size_t newline_pos = input.find('\n'); newline_pos != std::string::npos) {
        return input.substr(0, newline_pos);
    }
    // If no newline is found, return the entire string
    return input;
}

std::vector<std::string> get_task(const std::string& target, std::string& file_name) {
    info_out(1);
    printf("Getting tasks for task: %s\n", target.c_str());
    YAML::Node tasks = yml_paser(file_name);

    if (!tasks) {
        return {};
    }
    // Note: Because the var target always has some unknown char, need to clean.
    const std::string cleaned_target = cleaner(target);
    std::vector<std::string> task;

    if (!tasks.IsMap()){
        info_out(3);
        printf("Invalid tasks format in configuration file. Stop.\n");
        return task;
    }
    if (tasks[cleaned_target].IsNull()) {
        info_out(3);
        printf("Unknown task. Stop.\n");
        return task;
    }
    
    YAML::Node list = tasks[cleaned_target];

    for (int i = 0; i < list.size(); i++) {
        /* Add task */
        info_out(1);
        printf("Adding task: %s\n", list[i].as<std::string>().c_str());
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
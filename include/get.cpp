#include "main.h"

YAML::Node yml_paser(const std::string file_name) {

    
    YAML::Node config;

    try {
        config = YAML::LoadFile(file_name);
    }
    catch (const YAML::BadFile&) {}

    if (config.IsNull()) {
        print_status(1);
        printf("Configuration file not found. Stop.\n");
        return {};
    }

    YAML::Node tasks = config["tasks"];
    if (!tasks) {
        print_status(1);
        printf("'tasks' key not found in configuration file. Stop.\n");
        return {};
    }

    // Check if default in tasks
    if (!tasks["default"]) {
        print_status(2);
        printf("No 'default' task found. Continuing execution.\n");
    }

    return tasks;
}

bool command_parser(const std::string& command) {
    if (command.find("tasks.") != std::string::npos) {
        return false;
    }
    return true;
}

std::vector<std::string> get_task(const std::string& target) {
    static const std::vector<std::string> files = { 
        "maker.yml", "Maker.yml", "maker.yaml", "Maker.yaml" 
    };
    YAML::Node tasks;
    for(auto& item:files){
        tasks = yml_paser(item);
        if(tasks){
            break;
        }
    }

    if (!tasks) {
        return {};
    }
    std::vector<std::string> task;

    if (!tasks.IsMap()){
        print_status(1);
        printf("Invalid tasks format in configuration file. Stop.\n");
        return task;
    }
    if (tasks[target].IsNull()) {
        print_status(1);
        printf("Unknown task. Stop.\n");
        return task;
    }
    
    YAML::Node list = tasks[target];

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
    result.make_target = "";
    result.exit_code = 0;
    result.should_exit = false;
    result.command = cmd::NO_COMMAND;

    if (argc == 1) {
        print_status(0);
        printf("No action input. Stop.\n");
        result.exit_code = 1;
        result.should_exit = true;
        return result;
    }

    const std::string main_command = argv[1];
    auto cmd_iterator = g_command_map.find(main_command);

    if (cmd_iterator == g_command_map.end()) {
        print_status(0);
        printf("Invalid argument: %s\n", argv[1]);
        result.exit_code = 1;
        result.should_exit = true;
        return result;
    }

    result.command = cmd_iterator->second;

    switch (result.command) {
        case cmd::HELP:
            usage();
            result.should_exit = true;
            break;

        case cmd::VERSION:
            about();
            result.should_exit = true;
            break;

        case cmd::MAKE:
            print_status(2);
            if (argc == 2) {
                printf("No task input. Using default task.\n");
                result.make_target = "default";
            } else {
                result.make_target = argv[2];
            }
            break;
        case cmd::FORCE_LEGACY:
            print_status(2);
            printf("Force using legacy mode.\n");
        default:
            break;
    }

    return result;
}
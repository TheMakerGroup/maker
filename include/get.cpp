#include "main.h"

YAML::Node yml_paser(std::string& file_name) {

    const std::vector<std::string> files = { "maker.yml", "Maker.yml", "maker.yaml", "Maker.yaml" };
    YAML::Node config;

    for (const auto& filename : files) {
        try {
            file_name = filename;
            config = YAML::LoadFile(filename);
            break;
        }
        catch (const YAML::BadFile&) {}
    }

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

std::string command_paser(const std::string& command) {
    if (command.find("tasks.") != std::string::npos) {
        std::string target = command.substr(6, command.length());
        return target;
    }
    return "";
}

std::vector<std::string> get_task(const std::string& target, std::string& file_name) {
    YAML::Node tasks = yml_paser(file_name);

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

    for (int i = 0; i < list.size(); i++) {
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

std::string parser_arg(int argc, char** argv, int& status) {
    std::string target;
    switch (argc) {
    case 1:
        print_status(1);
        printf("No action input. Stop.\n");
        status = 1;
        return "";

    case 2:
        if (strcmp(argv[1], "make") == 0) {
            print_status(2);
            printf("No task input. Using default task.\n");
            target = "default";
            break;
        }
        else if (strcmp(argv[1], "-h") == 0) {
            usage();
            return "";
        }
        else if (strcmp(argv[1], "-v") == 0) {
            about();
            return "";
        }
        else {
            print_status(1);
            printf("Invalid argument: %s\n", argv[1]);
            status = 1;
            return "";
        }

    case 3:
        if (strcmp(argv[1], "make") == 0) {
            target = argv[2];
            break;
        }
        else if (strcmp(argv[1], "-h") == 0) {
            usage();
            return "";
        }
        else if (strcmp(argv[1], "-v") == 0) {
            about();
            return "";
        }
        else {
            print_status(1);
            printf("Invalid argument: %s\n", argv[1]);
            status = 1;
            return "";
        }

    default:
        print_status(1);
        printf("Too many arguments. Stop.\n");
        status = 1;
        return "";
    }
    return target;
}

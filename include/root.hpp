#pragma once

#include <yaml-cpp/yaml.h>

namespace maker {
    class execute;
    namespace execute_tool{};
    namespace print{};
    namespace get{};
    namespace analyze{};
    namespace init_cfg{
        inline bool should_exit;
        inline bool is_err;
    };
    inline YAML::Node root;
}
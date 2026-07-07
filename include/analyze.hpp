#include "root.hpp"
#include <yaml-cpp/yaml.h>
#include <string>
#include <utility>
#include <vector>

namespace maker::analyze {
    bool is_up_to_date(const std::string& source, const std::string& out);

    std::vector<std::pair<std::string, std::string>> get_paramers(const YAML::Node& task);
    bool need_execute(const std::string& task_name);

    inline YAML::Node root;
}
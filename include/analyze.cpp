#include <cstddef>
#include <filesystem>
#include <stdexcept>
#include <utility>
#include <vector>

#include "analyze.hpp"
#include "yaml-cpp/yaml.h"

namespace maker::analyze{
bool is_up_to_date(const std::string &source, const std::string &out){
    std::filesystem::file_time_type source_time, out_time;
    try{
        source_time = std::filesystem::last_write_time(source);
        out_time = std::filesystem::last_write_time(out);
    }catch(std::filesystem::filesystem_error){
        return false;
    }

    if(source_time > out_time){
        return false;
    }else{
        return true;
    }

}

std::vector<std::pair<std::string, std::string>> get_paramers(const YAML::Node& task){
    auto input = task["input"];
    auto output = task["output"];
    if(!input.IsSequence() || !output.IsSequence()){
        throw std::runtime_error("Unrecognized input or output format. Stop.\n");
    }
    if(input.size() != output.size()){
        throw std::runtime_error("Mismatched number of input and output elements. Stop.\n");
    }

    std::vector<std::pair<std::string, std::string>> in_out;
    in_out.reserve(input.size());
    for(size_t i = 0;i < input.size(); i++){
        auto pair = std::make_pair(input[i].as<std::string>(), output[i].as<std::string>());
        in_out.push_back(std::move(pair));
    }

    return in_out;
}

bool need_execute(const std::string &task_name, const YAML::Node& root){
    std::vector<std::pair<std::string, std::string>> in_out;
    in_out = get_paramers(root[task_name]);

    for(const auto& item : in_out){
        if(!is_up_to_date(item.first, item.second)){
            return true;
        }
    }
    return false;
}
}
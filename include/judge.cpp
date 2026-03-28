#include "judge.h"
#include "main.h"
#include "print.h"

// false means old style, true means new style
auto old_style_judge(const YAML::Node& node) -> bool {
    if (!node.IsMap()) { 
        return false;
}
    //judge input
    if (!node["input"].IsDefined()) { 
        return false;
}
    //judge output
    if (!node["output"].IsDefined()) { 
        return false;
}
    //judge cmd
    if (!node["cmd"].IsDefined()) { 
        return false;
}
    //here judge pass
    return true;
}

std::vector<std::vector<std::string>> get_in_out_cmd(const YAML::Node& node) {
    
    std::vector<std::string> input;
    std::vector<std::string> output;
    std::vector<std::string> cmd;

    std::vector<std::vector<std::string>> result;

    if (!old_style_judge(node)){ //Old style do not includes these part
        /* input get */
        YAML::Node input = node["input"];
        if (input && input.IsSequence()){
            for(int i = 0; i < input.size(); i++){
                input.push_back(input[i].as<std::string>());
            }
        }

        YAML::Node out = node["output"];
        if (out && out.IsSequence()){
            for(int i = 0; i < out.size(); i++){
                input.push_back(out[i].as<std::string>());
            }
        }

        YAML::Node cmd = node["cmd"];
        if (cmd && cmd.IsSequence()){
            for(int i = 0; i < cmd.size(); i++){
                cmd.push_back(cmd[i].as<std::string>());
            }
        }
    } else {
        for(int i = 0; i < node.size(); i++){
            cmd.push_back(node[i].as<std::string>());
        }
    }
    
    result.push_back(input);
    result.push_back(output);
    result.push_back(cmd);

    return result;
}

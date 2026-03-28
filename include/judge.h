#pragma once

#include "main.h"

// Note: the node only support the target key.
bool old_style_judge(const YAML::Node& node);

std::vector<std::vector<std::string>> get_in_out_cmd(const YAML::Node& node);
// Note: The struct:
/*[ ["a", "b"], ["c", "d"], ["e", "f"] ]
  ^ ^           ^           ^
  | |           |           | 
  | in vector   out vector  cmd vector
  wrap vector
The in refers to input, out refers to output.
*/

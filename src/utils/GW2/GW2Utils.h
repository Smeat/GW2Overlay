#ifndef __GW2UTILS_H_
#define __GW2UTILS_H_

#include "../json/json.hpp"
using json = nlohmann::json;

std::string json_build_to_string(json j_obj);

#endif

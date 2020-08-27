#include "GW2Achievements.h"

#include "../json/json.hpp"

#include <algorithm>
#include <iostream>

using json = nlohmann::json;

#define SET_FROM_JSON(e, n, v) \
	if (e.contains(n)) v = element[n];

GW2Achievements::GW2Achievements(const std::string& data) {
	auto j = json::parse(data);
	for (const auto& element : j) {
		GW2Achievement ach;
		ach.m_id = element["id"];
		if (element.contains("bits")) {
			for (const auto& bit : element["bits"]) {
				ach.m_bits.push_back(bit);
			}
		}
		SET_FROM_JSON(element, "current", ach.m_current);
		SET_FROM_JSON(element, "max", ach.m_max);
		SET_FROM_JSON(element, "done", ach.m_done);
		SET_FROM_JSON(element, "repeated", ach.m_repeated);
		SET_FROM_JSON(element, "unlocked", ach.m_unlocked);
		this->m_achievements.insert({ach.m_id, ach});
	}
}

bool GW2Achievements::is_done(int id) {
	auto res = this->m_achievements.find(id);
	return res != this->m_achievements.end() && res->second.m_done;
}

bool GW2Achievements::has_bit(int id, int bit) {
	auto res = this->m_achievements.find(id);
	return res != this->m_achievements.end() &&
		   (std::find(res->second.m_bits.begin(), res->second.m_bits.end(), bit) != res->second.m_bits.end());
}

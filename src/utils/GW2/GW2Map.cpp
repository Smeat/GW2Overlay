#include "GW2Map.h"

#include <utils/json/json.hpp>

#include <iostream>

void GW2Map::load_map(const std::string& json_string) {
	try {
		auto json_data = nlohmann::json::parse(json_string);
		this->m_id = json_data["id"];
		this->m_name = json_data["name"];
		this->m_min_level = json_data["min_level"];
		this->m_max_level = json_data["max_level"];
		this->m_default_floor = json_data["default_floor"];
		this->m_region_id = json_data["region_id"];
		this->m_region_name = json_data["region_name"];
		this->m_continent_id = json_data["continent_id"];
		this->m_continent_name = json_data["continent_name"];

		for (auto iter = json_data["floors"].begin(); iter != json_data["floors"].end(); ++iter) {
			this->m_floors.push_back(*iter);
		}

		int i = 0;
		for (auto iter = json_data["map_rect"].begin(); iter != json_data["map_rect"].end(); ++iter) {
			this->m_map_rect[i][0] = *(iter->begin());
			this->m_map_rect[i][1] = *(iter->begin() + 1);
			++i;
		}
		i = 0;
		for (auto iter = json_data["continent_rect"].begin(); iter != json_data["continent_rect"].end(); ++iter) {
			this->m_continent_rect[i][0] = *(iter->begin());
			this->m_continent_rect[i][1] = *(iter->begin() + 1);
			++i;
		}

	} catch (...) {
		// TODO: catch correct exception
		std::cout << "Failed to parse json data" << std::endl;
	}
}

template <typename T>
T map_value(T a1, T a2, T b1, T b2, T value) {
	return (b1 + ((value - a1) * (b2 - b1) / (a2 - a1)));
}

constexpr double toProperUnitFactor = 39.3700787;

float GW2Map::continentToMapX(float x) {
	return map_value(this->m_continent_rect[0][0], this->m_continent_rect[1][0], this->m_map_rect[0][0],
					 this->m_map_rect[1][0], x) /
		   toProperUnitFactor;
}
float GW2Map::continentToMapY(float x) {
	return (-1 * map_value(this->m_continent_rect[0][1], this->m_continent_rect[1][1], this->m_map_rect[0][1],
						   this->m_map_rect[1][1], x)) /
		   toProperUnitFactor;
}

float GW2Map::mapToContinentX(float x) {
	return map_value(this->m_map_rect[0][0], this->m_map_rect[1][0], this->m_continent_rect[0][0],
					 this->m_continent_rect[1][0], x) *
		   toProperUnitFactor;
}

float GW2Map::mapToContinentY(float y) {
	return map_value(this->m_map_rect[0][1], this->m_map_rect[1][1], this->m_continent_rect[0][1],
					 this->m_continent_rect[1][1], y) *
		   toProperUnitFactor;
}

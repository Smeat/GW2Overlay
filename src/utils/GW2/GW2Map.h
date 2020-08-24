#ifndef __GW2MAP_H__
#define __GW2MAP_H__

#include <string>
#include <vector>

class GW2Map {
 public:
	GW2Map() = default;

	void load_map(const std::string& json_data);
	float continentToMapX(float x);
	float continentToMapY(float x);
	float get_map_rect(int a, int b) { return this->m_map_rect[a][b]; }
	float get_continent_rect(int a, int b) { return this->m_continent_rect[a][b]; }

 private:
	int m_id;
	std::string m_name;
	int m_min_level;
	int m_max_level;
	int m_default_floor;
	std::vector<int> m_floors;
	int m_region_id;
	std::string m_region_name;
	int m_continent_id;
	std::string m_continent_name;
	int m_map_rect[2][2];
	int m_continent_rect[2][2];
};

#endif

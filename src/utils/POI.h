#ifndef __POI_H__
#define __POI_H__

#include <string>

#include <glm/vec3.hpp>

struct MarkerCategory {
	std::string m_name;
	std::string m_display_name;
	std::string m_icon_file;
	float m_icon_size = 1.0f;
	float m_height_offset = 0.0f;
};

struct POI {
	std::string m_type;
	int m_map_id;
	glm::vec3 m_pos;
};

#endif

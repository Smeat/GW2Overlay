#ifndef __POI_H__
#define __POI_H__

#include <cstring>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

#include <glm/vec3.hpp>

struct MarkerCategory {
	struct my_hash {
		size_t operator()(const std::shared_ptr<MarkerCategory>& v) const {
			size_t h = std::hash<std::string>{}(v->m_name);
			return h;
		};
	};
	struct my_comp {
		bool operator()(const std::shared_ptr<MarkerCategory>& a,
						const std::shared_ptr<MarkerCategory>& b) const {
			return *a == *b;
		}
	};
	typedef std::unordered_set<std::shared_ptr<MarkerCategory>, my_hash,
							   my_comp>
		category_container;
	std::string m_name;
	std::string m_display_name;
	std::string m_icon_file;
	float m_icon_size = 1.0f;
	float m_height_offset = 0.0f;
	bool m_active = true;
	category_container m_children;

	bool operator==(const MarkerCategory& other) {
		return this->m_name == other.m_name;
	}
	bool operator!=(const MarkerCategory& other) {
		return !this->operator==(other);
	}

	std::shared_ptr<MarkerCategory> get_child(const std::string& name);
	static std::shared_ptr<MarkerCategory> find_children(
		const category_container children, const std::string& name);
};

struct POI {
	std::string m_type;
	int m_map_id;
	glm::vec3 m_pos;
};

typedef std::vector<POI> poi_container;
typedef MarkerCategory::category_container category_container;

#endif

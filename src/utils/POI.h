#ifndef __POI_H__
#define __POI_H__

#include <cstring>
#include <iostream>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include <glm/vec3.hpp>

struct MarkerCategory;

typedef std::set<std::shared_ptr<MarkerCategory>> category_container;

struct MarkerCategory {
	std::string m_name;
	std::string m_display_name;
	std::string m_icon_file;
	float m_icon_size = 1.0f;
	float m_height_offset = 0.0f;
	bool m_active = true;
	category_container m_children;

	std::shared_ptr<MarkerCategory> get_child(const std::string& name) {
		std::vector<std::string> tokens;
		std::stringstream ss(name);
		std::string s;

		while (std::getline(ss, s, '.')) {
			tokens.push_back(s);
		}
		// not a valid child token
		if (tokens.size() <= 1) {
			return nullptr;
		}

		// striping own name
		std::string next_name = tokens[1];
		std::string next_string = name.substr(name.find_first_of(".") + 1);
		for (auto iter = this->m_children.begin();
			 iter != this->m_children.end(); ++iter) {
			// full match!
			if (next_string == (*iter)->m_name) {
				return *iter;
			}
			// partial match. search children
			else if (next_name == (*iter)->m_name) {
				return (*iter)->get_child(next_string);
			}
		}
		return nullptr;
	}

	static std::shared_ptr<MarkerCategory> find_children(
		const category_container children, const std::string& name) {
		for (auto iter = children.begin(); iter != children.end(); ++iter) {
			auto child = (*iter)->get_child(name);
			if (child) return child;
		}
		return nullptr;
	}
};

struct POI {
	std::string m_type;
	int m_map_id;
	glm::vec3 m_pos;
};

typedef std::vector<POI> poi_container;

#endif

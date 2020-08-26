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

enum poiBehavior {
	DEFAULT = 0,
	REAPPEAR_ON_MAP_CHANGE = 1,
	REAPPEAR_ON_DAILY_RESET = 2,
	ONLY_VISIBLE_BEFORE_ACTIVATION = 3,
	REAPPEAR_AFTER_TIMER = 4,
	REAPPEAR_ON_MAP_RESET = 5,
	ONCE_PER_INSTANCE = 6,
	ONCE_DAILY_PER_CHARACTER = 7
};

// TODO: are POI and MarkerCategory effectively the same?
struct POI {
	struct my_hash {
		size_t operator()(const std::shared_ptr<POI>& v) const {
			size_t h = std::hash<std::string>{}(v->m_name);
			return h;
		};
	};
	struct my_comp {
		bool operator()(const std::shared_ptr<POI>& a, const std::shared_ptr<POI>& b) const { return *a == *b; }
	};
	typedef std::unordered_set<std::shared_ptr<POI>, my_hash, my_comp> poi_container;
	std::string m_name;
	bool m_enabled = true;

	bool operator==(const POI& other) {
		return this->m_name == other.m_name && this->m_type == other.m_type && this->m_pos == other.m_pos &&
			   this->m_guid == other.m_guid;
	}
	bool operator!=(const POI& other) { return !this->operator==(other); }

	const poi_container* get_children() const;
	std::shared_ptr<POI> get_child(const std::string& name);
	static std::shared_ptr<POI> find_children(const poi_container children, const std::string& name);
	static std::shared_ptr<POI> create_child(const std::shared_ptr<POI> parent);

	std::string m_type;
	int m_map_id;
	glm::vec3 m_pos;
	std::string m_icon_file;
	std::string m_guid;
	float m_icon_size = 1.0f;
	float m_alpha = 1.0f;
	int m_behavior = poiBehavior::DEFAULT;
	float m_fade_near = -1;
	float m_fade_far = -1;
	float m_height_offset = 0;
	float m_reset_length = 0;
	std::string m_display_name;
	bool m_auto_trigger = false;
	bool m_has_countdown = false;
	float m_trigger_range = 0;
	// TODO: max/min size
	int m_achievement_id = 0;
	std::vector<int> m_achievement_bits;
	std::string m_info;
	float m_info_range = 0;
	bool m_is_poi = false;

	std::shared_ptr<POI> m_parent;
	poi_container m_children;
};

typedef POI::poi_container poi_container;

#endif

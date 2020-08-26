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
		bool operator()(const std::shared_ptr<MarkerCategory>& a, const std::shared_ptr<MarkerCategory>& b) const {
			return *a == *b;
		}
	};
	typedef std::unordered_set<std::shared_ptr<MarkerCategory>, my_hash, my_comp> category_container;
	std::string m_name;
	std::string m_display_name;
	std::string m_icon_file;
	float m_icon_size = 1.0f;
	float m_height_offset = 0.0f;
	bool m_enabled = true;
	category_container m_children;

	bool operator==(const MarkerCategory& other) { return this->m_name == other.m_name; }
	bool operator!=(const MarkerCategory& other) { return !this->operator==(other); }

	const category_container* get_children() const;
	std::shared_ptr<MarkerCategory> get_child(const std::string& name);
	static std::shared_ptr<MarkerCategory> find_children(const category_container children, const std::string& name);
};

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

	std::shared_ptr<MarkerCategory> m_parent;
};

typedef std::vector<std::shared_ptr<POI>> poi_container;
typedef MarkerCategory::category_container category_container;

#endif

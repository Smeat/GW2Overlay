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
	ONCE_DAILY_PER_CHARACTER = 7,
	ACTION_ON_COMBAT = 23732  // custom value.
};

// TODO: are POI and MarkerCategory effectively the same?
struct POI {
	virtual ~POI() = default;
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

	void set_parent(std::shared_ptr<POI> parent) { this->m_parent = parent; }
	// adds a new child and return it. If the child already exists: Return the existing child
	std::shared_ptr<POI> add_child(std::shared_ptr<POI> child) {
		auto ret = this->m_children.insert(child);
		return *std::get<0>(ret);
	}
	const poi_container* get_children() const;
	std::shared_ptr<POI> get_child(const std::string& name);
	static std::shared_ptr<POI> find_children(const poi_container children, const std::string& name);
	static std::shared_ptr<POI> create_child(const std::shared_ptr<POI> parent);

	// setter
	void set_name(const std::string& name);
	void set_map_id(int id);
	void set_x(float val);
	void set_y(float val);
	void set_z(float val);
	void set_type(const std::string& type);
	void set_guid(const std::string& uid);
	void set_icon_size(float val);
	void set_icon_file(const std::string& file);
	void set_alpha(float alpha);
	void set_behavior(int i);
	void set_fade_near(float fade);
	void set_fade_far(float fade);
	void set_height_offset(float off);
	void set_reset_length(int len);
	void set_display_name(const std::string& name);
	void set_auto_trigger(bool mode);
	void set_trigger_range(float range);
	void set_has_countdown(bool cd);
	void set_achievement_id(int id);
	void set_achievement_bit(int bit);
	void set_info(const std::string& info);
	void set_info_range(float range);
	void set_is_poi(bool poi);
	void set_pos(const glm::vec3& pos);

	// getter
	float get_icon_size() const;
	std::string get_icon_file() const;
	float get_alpha() const;
	int get_behavior() const;
	float get_fade_near() const;
	float get_fade_far() const;
	float get_height_offset() const;
	int get_reset_length() const;
	std::string get_display_name() const;
	bool get_auto_trigger() const;
	float get_trigger_range() const;
	bool get_has_countdown() const;
	int get_achievement_id() const;
	int get_achievement_bit() const;
	std::string get_info() const;
	float get_info_range() const;
	bool get_is_poi() const;
	int get_map_id() const;
	glm::vec3 get_pos() const;

 protected:
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
	float m_trigger_range = 5;
	// TODO: max/min size
	int m_achievement_id = 0;
	int m_achievement_bit = -1;
	std::string m_info;
	float m_info_range = 0;
	bool m_is_poi = false;

	std::shared_ptr<POI> m_parent;
	poi_container m_children;
};

typedef POI::poi_container poi_container;

#endif

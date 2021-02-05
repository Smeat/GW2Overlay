#include "POI.h"

std::shared_ptr<POI> POI::get_child(const std::string& name) {
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
	for (auto iter = this->m_children.begin(); iter != this->m_children.end(); ++iter) {
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

std::shared_ptr<POI> POI::find_children(const poi_container children, const std::string& name) {
	for (auto iter = children.begin(); iter != children.end(); ++iter) {
		auto child = (*iter)->get_child(name);
		if (child) return child;
	}
	return nullptr;
}
const poi_container* POI::get_children() const { return &this->m_children; }
void POI::clear_children() { this->m_children.clear(); }

// setter
void POI::set_name(const std::string& name) { this->m_name = name; }
void POI::set_map_id(int id) { this->m_inheritable_data.m_map_id = id; }
void POI::set_x(float val) { this->m_inheritable_data.m_pos.x = val; }
void POI::set_y(float val) { this->m_inheritable_data.m_pos.y = val; }
void POI::set_z(float val) { this->m_inheritable_data.m_pos.z = val; }
void POI::set_type(const std::string& type) { this->m_inheritable_data.m_type = type; }
void POI::set_guid(const std::string& uid) { this->m_inheritable_data.m_guid = uid; }
void POI::set_icon_size(float val) { this->m_inheritable_data.m_icon_size = val; }
void POI::set_icon_file(const std::string& file) { this->m_inheritable_data.m_icon_file = file; }
void POI::set_alpha(float alpha) { this->m_inheritable_data.m_alpha = alpha; }
void POI::set_behavior(int i) { this->m_inheritable_data.m_behavior = i; }
void POI::set_fade_near(float fade) { this->m_inheritable_data.m_fade_near = fade; }
void POI::set_fade_far(float fade) { this->m_inheritable_data.m_fade_far = fade; }
void POI::set_height_offset(float off) { this->m_inheritable_data.m_height_offset = off; }
void POI::set_reset_length(int len) { this->m_inheritable_data.m_reset_length = len; }
void POI::set_display_name(const std::string& name) { this->m_inheritable_data.m_display_name = name; }
void POI::set_auto_trigger(bool mode) { this->m_inheritable_data.m_auto_trigger = mode; }
void POI::set_trigger_range(float range) { this->m_inheritable_data.m_trigger_range = range; }
void POI::set_has_countdown(bool cd) { this->m_inheritable_data.m_has_countdown = cd; }
void POI::set_achievement_id(int id) { this->m_inheritable_data.m_achievement_id = id; }
void POI::set_achievement_bit(int bit) { this->m_inheritable_data.m_achievement_bit = bit; }
void POI::set_info(const std::string& info) { this->m_inheritable_data.m_info = info; }
void POI::set_info_range(float range) { this->m_inheritable_data.m_info_range = range; }
void POI::set_is_poi(bool poi) { this->m_inheritable_data.m_is_poi = poi; }
void POI::set_pos(const glm::vec3& pos) { this->m_inheritable_data.m_pos = pos; }

// getter
float POI::get_icon_size() const { return this->m_inheritable_data.m_icon_size; }
std::string POI::get_icon_file() const { return this->m_inheritable_data.m_icon_file; }
float POI::get_alpha() const { return this->m_inheritable_data.m_alpha; }
int POI::get_behavior() const { return this->m_inheritable_data.m_behavior; }
float POI::get_fade_near() const { return this->m_inheritable_data.m_fade_near; }
float POI::get_fade_far() const { return this->m_inheritable_data.m_fade_far; }
float POI::get_height_offset() const { return this->m_inheritable_data.m_height_offset; }
int POI::get_reset_length() const { return this->m_inheritable_data.m_reset_length; }
std::string POI::get_display_name() const { return this->m_inheritable_data.m_display_name; }
bool POI::get_auto_trigger() const { return this->m_inheritable_data.m_auto_trigger; }
float POI::get_trigger_range() const { return this->m_inheritable_data.m_trigger_range; }
bool POI::get_has_countdown() const { return this->m_inheritable_data.m_has_countdown; }
int POI::get_achievement_id() const { return this->m_inheritable_data.m_achievement_id; }
int POI::get_achievement_bit() const { return this->m_inheritable_data.m_achievement_bit; }
std::string POI::get_info() const { return this->m_inheritable_data.m_info; }
float POI::get_info_range() const { return this->m_inheritable_data.m_info_range; }
bool POI::get_is_poi() const { return this->m_inheritable_data.m_is_poi; }
int POI::get_map_id() const { return this->m_inheritable_data.m_map_id; }
glm::vec3 POI::get_pos() const { return this->m_inheritable_data.m_pos; }
std::string POI::get_guid() const { return this->m_inheritable_data.m_guid; }
std::string POI::get_name() const { return this->m_name; }
std::string POI::get_type() const { return this->m_inheritable_data.m_type; }

#include "CategoryManager.h"
#include <algorithm>
#include <cctype>
#include <memory>

#include "POI.h"
#include "Trail.h"
#include "xml/pugixml.hpp"

void print_categories(const poi_container* cat, const std::string& prefix = "") {
	for (auto iter = cat->begin(); iter != cat->end(); ++iter) {
		std::cout << prefix << "Display: " << (*iter)->m_display_name << " Name: " << (*iter)->m_name
				  << " Children: " << (*iter)->get_children()->size() << std::endl;

		print_categories((*iter)->get_children(), prefix + "-");
	}
}
void fill_trail(Trail* poi, pugi::xml_node& node) {
	poi->m_anim_speed = node.attribute("animSpeed").as_float(poi->m_anim_speed);
	poi->m_icon_file = node.attribute("texture").as_string(poi->m_icon_file.c_str());
	poi->m_trail_filename = node.attribute("trailData").as_string(poi->m_icon_file.c_str());
	poi->load_trail_data();
}

void fill_poi(POI* poi, pugi::xml_node& node) {
	poi->m_name = node.attribute("name").as_string();
	poi->m_map_id = node.attribute("MapID").as_int();
	poi->m_pos.x = node.attribute("xpos").as_float();
	poi->m_pos.y = node.attribute("ypos").as_float();
	poi->m_pos.z = node.attribute("zpos").as_float();
	poi->m_type = node.attribute("type").value();
	poi->m_guid = node.attribute("GUID").value();
	poi->m_icon_size = node.attribute("iconSize").as_float(poi->m_icon_size);
	poi->m_icon_file = node.attribute("iconFile").as_string(poi->m_icon_file.c_str());
	poi->m_alpha = node.attribute("alpha").as_float(poi->m_alpha);
	poi->m_behavior = node.attribute("behavior").as_int(poi->m_behavior);
	poi->m_fade_near = node.attribute("fadeNear").as_float(poi->m_fade_near);
	poi->m_fade_far = node.attribute("fadeFar").as_float(poi->m_fade_far);
	poi->m_height_offset = node.attribute("heightOffset").as_float(poi->m_height_offset);
	poi->m_reset_length = node.attribute("resetLength").as_int(poi->m_reset_length);
	poi->m_display_name = node.attribute("DisplayName").as_string(poi->m_display_name.c_str());
	poi->m_auto_trigger = node.attribute("autoTrigger").as_bool(poi->m_auto_trigger);
	poi->m_trigger_range = node.attribute("triggerRange").as_float(poi->m_trigger_range);
	poi->m_has_countdown = node.attribute("hasCountdown").as_bool(poi->m_has_countdown);
	poi->m_achievement_id = node.attribute("achievementId").as_int(poi->m_achievement_id);
	poi->m_achievement_bit = node.attribute("achievementBit").as_int(poi->m_achievement_bit);
	poi->m_info = node.attribute("info").as_string(poi->m_info.c_str());
	poi->m_info_range = node.attribute("infoRange").as_float(poi->m_info_range);
}

void CategoryManager::load_taco_xml_categories(const std::string& filename) {
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(filename.c_str());
	// load textures

	std::function<void(pugi::xml_node, std::shared_ptr<POI>)> traverse_markers_func;
	traverse_markers_func = [&](pugi::xml_node node, std::shared_ptr<POI> parent) {
		if (node.name() == std::string("MarkerCategory")) {
			std::shared_ptr<POI> cat = POI::create_child(parent);
			fill_poi(cat.get(), node);
			std::transform(cat->m_name.begin(), cat->m_name.end(), cat->m_name.begin(),
						   [](unsigned char c) { return std::tolower(c); });
			if (parent) {
				auto ret = parent->m_children.insert(cat);
				parent = *std::get<0>(ret);

			} else {
				auto ret = this->m_pois.insert(cat);
				parent = *std::get<0>(ret);
			}
		}
		for (pugi::xml_node n : node.children()) {
			traverse_markers_func(n, parent);
		}
		return "";
	};

	traverse_markers_func(doc.child("OverlayData"), nullptr);
}
void CategoryManager::load_taco_xml_pois(const std::string& filename) {
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(filename.c_str());
	// load textures
	std::function<void(pugi::xml_node)> traverse_poi_func;
	traverse_poi_func = [&](pugi::xml_node node) {
		for (pugi::xml_node n : node.children()) {
			traverse_poi_func(n);
		}
		if (node.name() == std::string("POI")) {
			// TODO: inherit values from category
			auto parent = POI::find_children(this->m_pois, (node.attribute("type").value()));
			// TODO: handle loading pois before the categories (different files etc)
			if (!parent) return;
			auto poi = POI::create_child(parent);
			fill_poi(poi.get(), node);
			poi->m_is_poi = true;
			parent->m_children.insert(poi);
		} else if (node.name() == std::string("Trail")) {
			//	return;	 // FIXME: skip for now
			// TODO: inherit values from category
			auto parent = POI::find_children(this->m_pois, (node.attribute("type").value()));
			// TODO: handle loading pois before the categories (different files etc)
			if (!parent) return;
			auto poi = Trail::create_child(parent);
			fill_poi(poi.get(), node);
			fill_trail(poi.get(), node);
			poi->m_is_poi = true;
			parent->m_children.insert(poi);
		}
	};
	traverse_poi_func(doc.child("OverlayData"));
}
void CategoryManager::load_taco_xmls(const std::vector<std::string>& filenames) {
	for (auto iter = filenames.begin(); iter != filenames.end(); ++iter) {
		this->load_taco_xml_categories(*iter);
	}
	for (auto iter = filenames.begin(); iter != filenames.end(); ++iter) {
		this->load_taco_xml_pois(*iter);
	}
	std::cout << "Loaded all files!" << std::endl;
}

const poi_container* CategoryManager::get_pois() const { return &this->m_pois; }

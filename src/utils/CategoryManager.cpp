#include "CategoryManager.h"
#include <algorithm>
#include <cctype>
#include <memory>

#include "POI.h"
#include "Trail.h"
#include "xml/pugixml.hpp"

void print_categories(const poi_container* cat, const std::string& prefix = "") {
	for (auto iter = cat->begin(); iter != cat->end(); ++iter) {
		std::cout << prefix << "Display: " << (*iter)->get_display_name() << " Name: " << (*iter)->get_name()
				  << " Children: " << (*iter)->get_children()->size() << std::endl;

		print_categories((*iter)->get_children(), prefix + "-");
	}
}
void fill_trail(Trail* poi, pugi::xml_node& node) {
	poi->m_anim_speed = node.attribute("animSpeed").as_float(poi->m_anim_speed);
	poi->set_icon_file(node.attribute("texture").as_string(poi->get_icon_file().c_str()));
	poi->m_trail_filename = node.attribute("trailData").as_string(poi->get_icon_file().c_str());
	poi->load_map_id();
}

void fill_poi(POI* poi, pugi::xml_node& node) {
	poi->set_name(node.attribute("name").as_string());
	poi->set_map_id(node.attribute("MapID").as_int());
	poi->set_x(node.attribute("xpos").as_float());
	poi->set_y(node.attribute("ypos").as_float());
	poi->set_z(node.attribute("zpos").as_float());
	poi->set_type(node.attribute("type").value());
	poi->set_guid(node.attribute("GUID").value());
	poi->set_icon_size(node.attribute("iconSize").as_float(poi->get_icon_size()));
	poi->set_icon_file(node.attribute("iconFile").as_string(poi->get_icon_file().c_str()));
	poi->set_alpha(node.attribute("alpha").as_float(poi->get_alpha()));
	poi->set_behavior(node.attribute("behavior").as_int(poi->get_behavior()));
	poi->set_fade_near(node.attribute("fadeNear").as_float(poi->get_fade_near()));
	poi->set_fade_far(node.attribute("fadeFar").as_float(poi->get_fade_far()));
	poi->set_height_offset(node.attribute("heightOffset").as_float(poi->get_height_offset()));
	poi->set_reset_length(node.attribute("resetLength").as_int(poi->get_reset_length()));
	poi->set_display_name(node.attribute("DisplayName").as_string(poi->get_display_name().c_str()));
	poi->set_auto_trigger(node.attribute("autoTrigger").as_bool(poi->get_auto_trigger()));
	poi->set_trigger_range(node.attribute("triggerRange").as_float(poi->get_trigger_range()));
	poi->set_has_countdown(node.attribute("hasCountdown").as_bool(poi->get_has_countdown()));
	poi->set_achievement_id(node.attribute("achievementId").as_int(poi->get_achievement_id()));
	poi->set_achievement_bit(node.attribute("achievementBit").as_int(poi->get_achievement_bit()));
	poi->set_info(node.attribute("info").as_string(poi->get_info().c_str()));
	poi->set_info_range(node.attribute("infoRange").as_float(poi->get_info_range()));
}

void CategoryManager::load_taco_xml_categories(const std::string& filename) {
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(filename.c_str());
	// load textures

	std::function<void(pugi::xml_node, std::shared_ptr<POI>)> traverse_markers_func;
	traverse_markers_func = [&](pugi::xml_node node, std::shared_ptr<POI> parent) {
		if (node.name() == std::string("MarkerCategory")) {
			std::shared_ptr<POI> cat = POI::create_poi(parent);
			fill_poi(cat.get(), node);
			std::string lower_name = cat->get_name();
			std::transform(lower_name.begin(), lower_name.end(), lower_name.begin(),
						   [](unsigned char c) { return std::tolower(c); });
			cat->set_name(lower_name);
			if (parent) {
				parent = parent->add_child(cat);
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
			auto parent = POI::find_children(this->m_pois, (node.attribute("type").value()));
			// TODO: handle loading pois before the categories (different files etc)
			if (!parent) return;
			auto poi = POI::create_poi(parent);
			fill_poi(poi.get(), node);
			poi->set_is_poi(true);
			parent->add_child(poi);
		} else if (node.name() == std::string("Trail")) {
			//	return;	 // FIXME: skip for now
			// TODO: inherit values from category
			auto parent = POI::find_children(this->m_pois, (node.attribute("type").value()));
			// TODO: handle loading pois before the categories (different files etc)
			if (!parent) return;
			auto poi = Trail::create_trail(parent);
			fill_poi(poi.get(), node);
			fill_trail(poi.get(), node);
			poi->set_is_poi(true);
			parent->add_child(poi);
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

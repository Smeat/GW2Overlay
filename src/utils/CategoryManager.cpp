#include "CategoryManager.h"
#include <algorithm>
#include <cctype>
#include <memory>

#include "POI.h"
#include "xml/pugixml.hpp"

void print_categories(const category_container* cat, const std::string& prefix = "") {
	for (auto iter = cat->begin(); iter != cat->end(); ++iter) {
		std::cout << prefix << "Display: " << (*iter)->m_display_name << " Name: " << (*iter)->m_name
				  << " Children: " << (*iter)->get_children()->size() << std::endl;

		print_categories((*iter)->get_children(), prefix + "-");
	}
}

void CategoryManager::load_taco_xml(const std::string& filename) {
	std::cout << "Loading taco xml file " << filename << " we already have " << this->m_pois.size() << " pois and "
			  << this->m_categories.size() << " root categories" << std::endl;
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(filename.c_str());
	// load textures

	std::function<void(pugi::xml_node, std::shared_ptr<MarkerCategory>)> traverse_markers_func;
	traverse_markers_func = [&](pugi::xml_node node, std::shared_ptr<MarkerCategory> parent) {
		if (node.name() == std::string("MarkerCategory")) {
			std::shared_ptr<MarkerCategory> cat(new MarkerCategory);
			cat->m_icon_file = node.attribute("iconFile").value();
			cat->m_icon_size = node.attribute("iconSize").as_float(1.0f);
			cat->m_height_offset = node.attribute("heightOffset").as_float(0);
			cat->m_display_name = node.attribute("DisplayName").value();
			cat->m_name = node.attribute("name").value();
			std::transform(cat->m_name.begin(), cat->m_name.end(), cat->m_name.begin(),
						   [](unsigned char c) { return std::tolower(c); });
			if (parent) {
				auto ret = parent->m_children.insert(cat);
				parent = *std::get<0>(ret);

			} else {
				auto ret = this->m_categories.insert(cat);
				parent = *std::get<0>(ret);
			}
		}
		for (pugi::xml_node n : node.children()) {
			traverse_markers_func(n, parent);
		}
		return "";
	};

	std::function<void(pugi::xml_node)> traverse_poi_func;
	traverse_poi_func = [&](pugi::xml_node node) {
		for (pugi::xml_node n : node.children()) {
			traverse_poi_func(n);
		}
		if (node.name() == std::string("POI")) {
			// TODO: inherit values from category
			POI poi;
			poi.m_map_id = node.attribute("MapID").as_int();
			poi.m_pos.x = node.attribute("xpos").as_float();
			poi.m_pos.y = node.attribute("ypos").as_float();
			poi.m_pos.z = node.attribute("zpos").as_float();
			poi.m_type = node.attribute("type").value();
			// poi.m_icon_file = node.attribute("iconFile").value();
			poi.m_guid = node.attribute("GUID").value();
			poi.m_icon_size = node.attribute("iconSize").as_float(poi.m_icon_size);
			poi.m_alpha = node.attribute("alpha").as_float(poi.m_alpha);
			poi.m_behavior = node.attribute("behavior").as_int(poi.m_behavior);
			poi.m_fade_near = node.attribute("fadeNear").as_float(poi.m_fade_near);
			poi.m_fade_far = node.attribute("fadeFar").as_float(poi.m_fade_far);
			poi.m_height_offset = node.attribute("heightOffset").as_float(poi.m_height_offset);
			poi.m_reset_length = node.attribute("resetLength").as_int(poi.m_reset_length);
			poi.m_display_name = node.attribute("DisplayName").as_string(poi.m_display_name.c_str());
			poi.m_auto_trigger = node.attribute("autoTrigger").as_bool(poi.m_auto_trigger);
			poi.m_trigger_range = node.attribute("triggerRange").as_float(poi.m_trigger_range);
			poi.m_has_countdown = node.attribute("hasCountdown").as_bool(poi.m_has_countdown);
			poi.m_achievement_id = node.attribute("achievementId").as_int(poi.m_achievement_id);
			// TODO
			// poi.m_achievement_bits;
			poi.m_info = node.attribute("info").as_string(poi.m_info.c_str());
			poi.m_info_range = node.attribute("infoRange").as_float(poi.m_info_range);

			this->m_pois.push_back(std::make_shared<POI>(poi));
		}
	};
	traverse_markers_func(doc.child("OverlayData"), nullptr);
	traverse_poi_func(doc.child("OverlayData"));
}
void CategoryManager::load_taco_xmls(const std::vector<std::string>& filenames) {
	for (auto iter = filenames.begin(); iter != filenames.end(); ++iter) {
		this->load_taco_xml(*iter);
		//		print_categories(&this->m_categories);
	}
}

const category_container* CategoryManager::get_categories() const { return &this->m_categories; }
const poi_container* CategoryManager::get_pois() const { return &this->m_pois; }

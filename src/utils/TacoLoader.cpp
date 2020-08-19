#include "TacoLoader.h"

void load_xml_types(const std::string& filename, poi_container* poi_vec,
					category_container* category_vec) {
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(filename.c_str());
	// load textures

	std::function<void(pugi::xml_node, std::shared_ptr<MarkerCategory>)>
		traverse_markers_func;
	traverse_markers_func = [&](pugi::xml_node node,
								std::shared_ptr<MarkerCategory> parent) {
		if (node.name() == std::string("MarkerCategory")) {
			std::shared_ptr<MarkerCategory> cat(new MarkerCategory);
			cat->m_icon_file = node.attribute("iconFile").value();
			cat->m_icon_size = node.attribute("iconSize").as_float(1.0f);
			cat->m_height_offset = node.attribute("heightOffset").as_float(0);
			cat->m_display_name = node.attribute("DisplayName").value();
			cat->m_name = node.attribute("name").value();
			if (parent) {
				parent->m_children.insert(cat);
			} else {
				// "root" node
				category_vec->insert(cat);
			}
			// set new parent
			parent = cat;
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
			POI poi;
			poi.m_map_id = node.attribute("MapID").as_int();
			poi.m_pos.x = node.attribute("xpos").as_float();
			poi.m_pos.y = node.attribute("ypos").as_float();
			poi.m_pos.z = node.attribute("zpos").as_float();
			poi.m_type = node.attribute("type").value();
			poi_vec->push_back(poi);
		}
	};
	traverse_markers_func(doc.child("OverlayData"), nullptr);
	traverse_poi_func(doc.child("OverlayData"));
}

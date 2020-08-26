#ifndef __CATEGORY_MANAGER_H__
#define __CATEGORY_MANAGER_H__

#include <memory>
#include <vector>
#include "POI.h"

class CategoryManager {
 public:
	static CategoryManager& getInstance() {
		static CategoryManager instance;
		return instance;
	}
	void load_taco_xml_categories(const std::string& filename);
	void load_taco_xml_pois(const std::string& filename);
	void load_taco_xmls(const std::vector<std::string>& filenames);
	const poi_container* get_pois() const;

	// TODO: add mutex
	bool state_changed() { return this->m_state_changed; }
	void set_state_changed(bool state) { this->m_state_changed = state; }

 private:
	CategoryManager() = default;
	CategoryManager(CategoryManager const&);
	void operator=(CategoryManager const&);

	poi_container m_pois;

	// signals a changed state (e.g. activated)
	bool m_state_changed;
};

#endif	// __CATEGORY_MANAGER_H__

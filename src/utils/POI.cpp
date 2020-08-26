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


#include "GW2Object.h"

GW2Object::GW2Object(std::shared_ptr<Object> obj, std::shared_ptr<POI> poi) {
	this->m_object = obj;
	this->m_poi = poi;
}

void GW2Object::check_trigger(const glm::vec3& pos) {
	// Triggered action
	if (glm::distance(*this->m_object->get_position(), pos) <= this->m_poi->m_trigger_range) {
		if (this->m_poi->m_auto_trigger) {
			// XXX: just scale it down, since we'd need to rebuild the command queue otherwise
			this->m_object->scale({0, 0, 0});
		}
	}
}

#include "GW2Object.h"

GW2Object::GW2Object(std::shared_ptr<Object> obj, std::shared_ptr<POI> poi) {
	this->m_object = obj;
	this->m_poi = poi;
}

void GW2Object::check_trigger(const glm::vec3& pos, bool pressed_f) {
	// Triggered action
	if (glm::distance(*this->m_object->get_position(), pos) <= this->m_poi->m_trigger_range) {
		bool remove = this->m_poi->m_auto_trigger;
		// TODO: set flag in config for daily reset etc
		remove |= (this->m_poi->m_behavior == poiBehavior::REAPPEAR_ON_DAILY_RESET) ||
				  (this->m_poi->m_behavior == poiBehavior::ONLY_VISIBLE_BEFORE_ACTIVATION);
		if (remove) {
			// XXX: just scale it down, since we'd need to rebuild the command queue otherwise
			this->m_object->scale({0, 0, 0});
		}
	}
}

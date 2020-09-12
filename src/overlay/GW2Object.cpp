#include "GW2Object.h"

GW2POIObject::GW2POIObject(std::shared_ptr<POI> poi) { this->m_poi = poi; }

void GW2POIObject::update(const glm::vec3& pos, uint64_t button_mask) {
	// Triggered action
	if (glm::distance(this->get_world_position(), pos) <= this->m_poi->m_trigger_range) {
		bool remove = this->m_poi->m_auto_trigger;
		// TODO: set flag in config for daily reset etc
		remove |= ((this->m_poi->m_behavior == poiBehavior::REAPPEAR_ON_DAILY_RESET) ||
				   (this->m_poi->m_behavior == poiBehavior::ONLY_VISIBLE_BEFORE_ACTIVATION)) &&
				  button_mask;
		// TODO: use actual button mask
		if (remove) {
			// XXX: just scale it down, since we'd need to rebuild the command queue otherwise
			this->scale({0, 0, 0});
		}
	}
}

#include "GW2Object.h"

GW2POIObject::GW2POIObject(std::shared_ptr<POI> poi) { this->m_poi = poi; }

void GW2POIObject::update(const glm::vec3& pos, uint64_t button_mask) {
	// Triggered action
	float distance = glm::distance(this->get_world_position(), pos);
	if (distance <= this->m_poi->get_trigger_range()) {
		bool remove = this->m_poi->get_auto_trigger();
		// TODO: set flag in config for daily reset etc
		remove |= ((this->m_poi->get_behavior() == poiBehavior::REAPPEAR_ON_DAILY_RESET) ||
				   (this->m_poi->get_behavior() == poiBehavior::ONLY_VISIBLE_BEFORE_ACTIVATION)) &&
				  button_mask;
		// TODO: use actual button mask
		if (remove) {
			// XXX: just scale it down, since we'd need to rebuild the command queue otherwise
			this->scale({0, 0, 0});
			this->m_inactive = true;
			this->m_disabled = true;
		}
	}
	if (!this->m_disabled) {
		if ((this->m_poi->get_fade_far() > 0 && distance > this->m_poi->get_fade_far()) ||
			(this->m_poi->get_fade_near() > 0 && distance < this->m_poi->get_fade_near() && false)) {
			if (!this->m_inactive) {
				std::cout << "Disabling object " << this->m_poi->get_type() << " with distance " << distance << " far "
						  << this->m_poi->get_fade_far() << " near " << this->m_poi->get_fade_near() << std::endl;
				this->scale({0, 0, 0});
				this->m_inactive = true;
			}
		} else if (this->m_inactive) {
			std::cout << "Enabling object " << this->m_poi->get_type() << " with distance " << distance << " far "
					  << this->m_poi->get_fade_far() << " near " << this->m_poi->get_fade_near() << std::endl;
			this->scale({this->m_poi->get_icon_size() * 1.0f, this->m_poi->get_icon_size() * 1.0f, 1.0f});
			this->m_inactive = false;
		}
	}
}

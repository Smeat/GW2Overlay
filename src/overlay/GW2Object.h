#ifndef __GW2OBJECT_H__
#define __GW2OBJECT_H__

#include <glm/ext/vector_float3.hpp>
#include <memory>
#include <vector>

#include "../utils/POI.h"
#include "Object.h"

// TODO: add parent and local/world space functions
class GW2Object {
 public:
	virtual void update(const glm::vec3& player_pos, uint64_t button_mask) = 0;
	virtual std::vector<std::shared_ptr<Object>> get_objects() = 0;
	virtual void translate(const glm::vec3& pos) {
		for (const auto& o : this->get_objects()) {
			o->translate(pos);
		}
	}
	virtual void set_offset(const glm::vec3& pos) {
		for (const auto& o : this->get_objects()) {
			o->set_offset(pos);
		}
	}
};

class GW2POIObject : public GW2Object {
 public:
	GW2POIObject(std::shared_ptr<Object> obj, std::shared_ptr<POI> poi);

	virtual void update(const glm::vec3& pos, uint64_t button_mask) override;

	std::vector<std::shared_ptr<Object>> get_objects() override { return {this->m_object}; }

 private:
	std::shared_ptr<Object> m_object;
	std::shared_ptr<POI> m_poi;
};

#endif

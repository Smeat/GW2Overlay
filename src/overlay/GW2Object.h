#ifndef __GW2OBJECT_H__
#define __GW2OBJECT_H__

#include <glm/ext/vector_float3.hpp>
#include <memory>
#include <vector>

#include "../utils/POI.h"
#include "Object.h"

// TODO: add parent and local/world space functions
class GW2Object : public Object {
 public:
	GW2Object(){};
	virtual void update(const glm::vec3& player_pos, uint64_t button_mask) = 0;
};

class GW2POIObject : public GW2Object {
 public:
	GW2POIObject(std::shared_ptr<POI> poi);

	virtual void update(const glm::vec3& pos, uint64_t button_mask) override;

 private:
	std::shared_ptr<POI> m_poi;
	bool m_inactive = false;
	bool m_disabled = false;
};

#endif

#ifndef __GW2OBJECT_H__
#define __GW2OBJECT_H__

#include <glm/ext/vector_float3.hpp>
#include <memory>

#include "../utils/POI.h"
#include "Object.h"

class GW2Object {
 public:
	GW2Object(std::shared_ptr<Object> obj, std::shared_ptr<POI> poi);

	void check_trigger(const glm::vec3& pos);

	const std::shared_ptr<Object> get_object() const { return this->m_object; }

 private:
	std::shared_ptr<Object> m_object;
	std::shared_ptr<POI> m_poi;
};

#endif

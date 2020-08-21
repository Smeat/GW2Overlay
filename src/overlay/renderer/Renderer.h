#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <memory>
#include <vector>

#include "../Object.h"

class Renderer {
 public:
	virtual void init() = 0;
	virtual void update(std::vector<std::shared_ptr<Object>> objs) = 0;
	virtual void clear() = 0;

	virtual void allocate_mesh(std::shared_ptr<Mesh> v) = 0;
	virtual void allocate_texture(std::shared_ptr<Texture> t) = 0;
};

#endif	// __RENDERER_H__

#ifndef __GLRENDERER_H__
#define __GLRENDERER_H__

#include <memory>
#include "Renderer.h"

class GLRenderer : public Renderer {
 public:
	GLRenderer() { this->init(); }
	virtual void init() override;
	virtual void update(std::vector<std::shared_ptr<Object>> objs) override;
	virtual void clear() override;
	virtual void allocate_mesh(std::shared_ptr<Mesh> v) override;
	virtual void allocate_texture(std::shared_ptr<Texture> t) override;
};

#endif	// __GLRENDERER_H__

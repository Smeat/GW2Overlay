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
	virtual std::shared_ptr<Texture> load_texture(const std::string& path) override;
	virtual std::shared_ptr<Mesh> load_mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices) override;
};

#endif	// __GLRENDERER_H__

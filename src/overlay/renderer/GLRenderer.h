#ifndef __GLRENDERER_H__
#define __GLRENDERER_H__

#include <memory>
#include "../Window.h"
#include "Renderer.h"

class GLRenderer : public Renderer {
 public:
	GLRenderer(WindowData data) {
		this->m_window = data;
		this->init();
	}
	virtual void init() override;
	virtual void set_objects(std::vector<std::shared_ptr<Object>> objs) override;
	virtual void update() override;
	virtual void clear() override;
	virtual std::shared_ptr<Texture> load_texture(const std::string& path) override;
	virtual std::shared_ptr<Mesh> load_mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices) override;
	virtual std::shared_ptr<Shader> load_shader(const std::string& vert, const std::string& frag) override;

 private:
	std::vector<std::shared_ptr<Object>> m_objects;
	WindowData m_window;
};

#endif	// __GLRENDERER_H__

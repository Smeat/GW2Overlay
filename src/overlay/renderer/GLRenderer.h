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
	virtual void set_objects(std::vector<Object*> objs) override;
	virtual void update() override;
	virtual void clear() override;
	virtual std::shared_ptr<Texture> load_texture(SDL_Surface* surf) override;
	virtual std::shared_ptr<Mesh> load_mesh(VertexList vertices, IndexList indices) override;
	virtual std::shared_ptr<Shader> load_shader(const std::string& vert, const std::string& frag) override;
	virtual std::vector<Object*> get_objects() override { return {}; };

 private:
	std::vector<Object*> m_objects;
	WindowData m_window;
};

#endif	// __GLRENDERER_H__

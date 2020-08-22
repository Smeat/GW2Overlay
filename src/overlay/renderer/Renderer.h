#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <memory>
#include <string>
#include <vector>

#include "../Object.h"

class Renderer {
 public:
	virtual void init() = 0;
	virtual void clear() = 0;

	virtual void update() = 0;
	// TODO: add a notify method and use a pointer
	virtual void set_objects(std::vector<std::shared_ptr<Object>> objs) = 0;
	virtual std::shared_ptr<Texture> load_texture(const std::string& path) = 0;
	virtual std::shared_ptr<Mesh> load_mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices) = 0;
	virtual std::shared_ptr<Shader> load_shader(const std::string& vert, const std::string& frag) = 0;
	virtual std::shared_ptr<Object> load_object(std::shared_ptr<Shader> s,
												std::vector<std::shared_ptr<TexturedMesh>> m) {
		return std::shared_ptr<Object>(new Object(s, m));
	}
};

#endif	// __RENDERER_H__

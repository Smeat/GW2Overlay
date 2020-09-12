#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <SDL2/SDL_surface.h>
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
	virtual void set_objects(std::vector<Object*> objs) = 0;
	virtual std::vector<Object*> get_objects() = 0;
	virtual std::shared_ptr<Texture> load_texture(SDL_Surface* surf) = 0;
	virtual std::shared_ptr<Texture> load_texture(const std::string& path) {
		SDL_Surface* surf = Texture::load_image(path);
		auto tex = this->load_texture(surf);
		SDL_FreeSurface(surf);
		return tex;
	}
	virtual std::shared_ptr<Mesh> load_mesh(VertexList vertices, IndexList indices) = 0;
	virtual std::shared_ptr<Shader> load_shader(const std::string& vert, const std::string& frag) = 0;
};

#endif	// __RENDERER_H__

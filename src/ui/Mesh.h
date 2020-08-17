#ifndef __MESH_H__
#define __MESH_H__

#include "Texture.h"

#include <glm/fwd.hpp>
#include <vector>
#include <memory>

#include <glm/glm.hpp>

struct Vertex {
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 tex_coord;
	
	Vertex(glm::vec3 p, glm::vec3 c, glm::vec2 t) {
		this->pos = p;
		this->color = c;
		this->tex_coord = t;
	}
};

class Mesh {

	public:
		Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::shared_ptr<Texture> tex);
		void draw();
	private:
		std::vector<Vertex> m_vertices;
		std::vector<unsigned int> m_indices;
		std::shared_ptr<Texture> m_texture;
		unsigned int m_vao, m_vbo, m_ebo;
};

#endif

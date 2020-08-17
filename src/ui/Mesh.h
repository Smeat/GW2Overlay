/*
 * Copyright (c) 2020 smeat.
 *
 * This file is part of GW2Overlay 
 * (see https://github.com/Smeat/GW2Overlay).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
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

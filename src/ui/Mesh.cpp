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
#include "Mesh.h"

#include "Texture.h"

#include <GL/glew.h>

#include <GL/gl.h>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
		   std::shared_ptr<Texture> tex) {
	this->m_texture = tex;
	this->m_vertices = vertices;
	this->m_indices = indices;
	glGenVertexArrays(1, &this->m_vao);
	glGenBuffers(1, &this->m_vbo);
	glGenBuffers(1, &this->m_ebo);
	glBindVertexArray(this->m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_vbo);
	glBufferData(GL_ARRAY_BUFFER, this->m_vertices.size() * sizeof(Vertex),
				 &this->m_vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				 sizeof(this->m_indices.size()) * sizeof(unsigned int),
				 &this->m_indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
						  (void*)offsetof(Vertex, pos));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
						  (void*)offsetof(Vertex, color));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
						  (void*)offsetof(Vertex, tex_coord));

	glBindVertexArray(0);
}

Mesh::~Mesh() {
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &this->m_vbo);
	glDeleteBuffers(1, &this->m_ebo);
	glDeleteVertexArrays(1, &this->m_vao);
}

void Mesh::draw() {
	this->m_texture->set_active();
	glBindVertexArray(this->m_vao);
	glDrawElements(GL_TRIANGLES, this->m_indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

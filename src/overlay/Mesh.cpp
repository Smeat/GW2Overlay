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

Mesh::Mesh(VertexList vertices, IndexList indices) {
	this->m_vertices = std::make_shared<VertexList>(vertices);
	this->m_indices = std::make_shared<IndexList>(indices);
}

Mesh Mesh::create_default_mesh(glm::vec3 color) {
	std::vector<Vertex> vertices;
	vertices.push_back(Vertex(glm::vec3(-0.5f, -0.5f, 0.0f), color, glm::vec2(0.0f, 0.0f)));
	vertices.push_back(Vertex(glm::vec3(0.5f, -0.5f, 0.0f), color, glm::vec2(1.0f, 0.0f)));
	vertices.push_back(Vertex(glm::vec3(0.5f, 0.5f, 0.0f), color, glm::vec2(1.0f, 1.0f)));
	vertices.push_back(Vertex(glm::vec3(-0.5f, 0.5f, 0.0f), color, glm::vec2(0.0f, 1.0f)));

	return Mesh(vertices, {0, 1, 2, 2, 3, 0});
}

TexturedMesh::TexturedMesh(std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> tex) : m_mesh(mesh), m_texture(tex) {}

void TexturedMesh::draw() {
	this->m_texture->set_active();
	this->m_mesh->draw();
}


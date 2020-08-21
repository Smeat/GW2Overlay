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

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices) {
	this->m_vertices = std::make_shared<std::vector<Vertex>>(vertices);
	this->m_indices = std::make_shared<std::vector<unsigned int>>(indices);
}

TexturedMesh::TexturedMesh(std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> tex) : m_mesh(mesh), m_texture(tex) {}

void TexturedMesh::draw() {
	this->m_texture->set_active();
	this->m_mesh->draw();
}


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

#include <vulkan/vulkan.h>
#include <glm/fwd.hpp>
#include <memory>
#include <vector>

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
	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, tex_coord);

		return attributeDescriptions;
	}
};

class Mesh {
 public:
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
	virtual ~Mesh() = default;
	virtual void draw() = 0;
	const std::shared_ptr<std::vector<Vertex>> get_vertices() const { return this->m_vertices; }
	const std::shared_ptr<std::vector<unsigned int>> get_indices() const { return this->m_indices; }

 protected:
	std::shared_ptr<std::vector<Vertex>> m_vertices;
	std::shared_ptr<std::vector<unsigned int>> m_indices;
};

class TexturedMesh {
 public:
	TexturedMesh(std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> tex);
	void draw();
	const std::shared_ptr<Mesh> get_mesh() const { return this->m_mesh; }
	const std::shared_ptr<Texture> get_texture() const { return this->m_texture; }

 private:
	std::shared_ptr<Mesh> m_mesh;
	std::shared_ptr<Texture> m_texture;
};

#endif

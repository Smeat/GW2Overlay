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
#include <functional>
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
	Vertex() = default;
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

typedef std::vector<Vertex> VertexList;
typedef std::vector<uint16_t> IndexList;

class Mesh {
 public:
	Mesh(VertexList vertices, IndexList indices);
	virtual ~Mesh() = default;
	virtual void draw(){};
	const std::shared_ptr<VertexList> get_vertices() const { return this->m_vertices; }
	const std::shared_ptr<IndexList> get_indices() const { return this->m_indices; }

	static Mesh create_default_mesh(glm::vec3 color = {1, 1, 1});

 protected:
	std::shared_ptr<VertexList> m_vertices;
	std::shared_ptr<IndexList> m_indices;
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

template <>
struct std::hash<VertexList> {
	std::size_t operator()(const VertexList& m) const noexcept {
		// TODO: improve this hash function or leave it as a quick and dirty way?
		return m.size();
	}
};
template <>
struct std::hash<IndexList> {
	std::size_t operator()(const IndexList& m) const noexcept {
		// TODO: improve this hash function or leave it as a quick and dirty way?
		return m.size();
	}
};

template <>
struct std::hash<Mesh> {
	std::size_t operator()(const Mesh& m) const noexcept {
		auto h1 = std::hash<VertexList>{}(*m.get_vertices());
		auto h2 = std::hash<IndexList>{}(*m.get_indices());
		return h1 ^ (h2 << 1);
	}
};

template <>
struct std::hash<std::shared_ptr<Mesh>> {
	std::size_t operator()(const std::shared_ptr<Mesh> m) const noexcept {
		// use size as a quick "hash" to avoid comparisons
		return std::hash<Mesh>{}(*m);
	}
};

template <>
struct std::equal_to<VertexList> {
	bool operator()(const VertexList& a, const VertexList& b) const noexcept {
		if (a.size() != b.size()) return false;
		for (int i = 0; i < a.size(); ++i) {
			if (a[i].pos != b[i].pos || a[i].color != b[i].color || a[i].tex_coord != b[i].tex_coord) {
				return false;
			}
		}
		return true;
	}
};

template <>
struct std::equal_to<IndexList> {
	bool operator()(const IndexList& a, const IndexList& b) const noexcept {
		if (a.size() != b.size()) return false;
		for (int i = 0; i < a.size(); ++i) {
			if (a[i] != b[i]) {
				return false;
			}
		}
		return true;
	}
};

template <>
struct std::equal_to<std::shared_ptr<IndexList>> {
	bool operator()(const std::shared_ptr<IndexList> a, const std::shared_ptr<IndexList> b) const noexcept {
		return std::equal_to<IndexList>{}(*a, *b);
	}
};

template <>
struct std::equal_to<std::shared_ptr<VertexList>> {
	bool operator()(const std::shared_ptr<VertexList> a, const std::shared_ptr<VertexList> b) const noexcept {
		return std::equal_to<VertexList>{}(*a, *b);
	}
};

template <>
struct std::equal_to<Mesh> {
	bool operator()(const Mesh& a, const Mesh& b) const noexcept {
		return std::equal_to<std::shared_ptr<VertexList>>{}(a.get_vertices(), b.get_vertices()) &&
			   std::equal_to<std::shared_ptr<IndexList>>{}(a.get_indices(), b.get_indices());
	}
};

template <>
struct std::equal_to<std::shared_ptr<Mesh>> {
	bool operator()(const std::shared_ptr<Mesh> a, const std::shared_ptr<Mesh> b) const noexcept {
		return std::equal_to<Mesh>{}(*a, *b);
	}
};
#endif

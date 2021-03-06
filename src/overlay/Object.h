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
#ifndef __OBJECT__H__
#define __OBJECT__H__

#include <glm/fwd.hpp>
#include "Mesh.h"
#include "Shader.h"

#include <glm/trigonometric.hpp>
#include <memory>
#include <unordered_set>
#include <vector>

class Object {
 public:
	Object() = default;
	Object(std::shared_ptr<Shader> s, std::vector<std::shared_ptr<TexturedMesh>> vertex_data);
	virtual ~Object() = default;

	void set_shader(std::shared_ptr<Shader> s) { this->m_shader = s; };
	void set_meshes(std::vector<std::shared_ptr<TexturedMesh>> vertex_data) {
		this->m_meshes = std::make_shared<std::vector<std::shared_ptr<TexturedMesh>>>(vertex_data);
	}

	void add_child(Object* child) { this->m_children.insert(child); }
	void remove_child(Object* child) { this->m_children.erase(child); }
	void set_parent(Object* parent) {
		if (this->m_parent) this->m_parent->remove_child(this);
		this->m_parent = parent;
		if (parent) parent->add_child(this);
	}

	std::vector<Object*> get_all_children();

	void translate(glm::vec3 pos);
	void set_offset(glm::vec3 offset);
	void scale(glm::vec3 scale);
	void set_scale_factor(glm::vec3 fac);
	void rotate(float deg, glm::vec3 v);
	virtual void update();
	const std::shared_ptr<std::vector<std::shared_ptr<TexturedMesh>>> get_textured_meshes() const;
	const std::shared_ptr<Shader> get_shader() const { return this->m_shader; }
	glm::vec3 get_local_position() const { return this->m_pos + this->m_offset; }
	glm::vec3 get_world_position() const;

 protected:
	void update_model_matrix();
	std::shared_ptr<Shader> m_shader;
	std::shared_ptr<std::vector<std::shared_ptr<TexturedMesh>>> m_meshes;
	glm::mat4 m_model;

	Object* m_parent = nullptr;
	std::unordered_set<Object*> m_children;

	// position etc
	glm::vec3 m_pos = glm::vec3(0, 0, 0);
	glm::vec3 m_offset = glm::vec3(0);
	glm::vec3 m_scale = glm::vec3(1);
	glm::vec3 m_scale_factor = glm::vec3(1);
	glm::vec3 m_rotation_vec = glm::vec3(0.0f, 0.0f, 1.0f);
	float m_rotation = glm::radians(0.0f);
};

#endif	// __OBJECT__H__

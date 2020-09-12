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
#include "Object.h"
#include "Mesh.h"

#include <algorithm>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iterator>
#include <memory>

#include <GL/gl.h>

Object::Object(std::shared_ptr<Shader> s, std::vector<std::shared_ptr<TexturedMesh>> vertex_data) {
	this->m_shader = s;
	this->m_meshes = std::make_shared<std::vector<std::shared_ptr<TexturedMesh>>>(vertex_data);
}

void Object::translate(glm::vec3 pos) {
	this->m_pos = pos;
	this->update_model_matrix();
}
void Object::set_offset(glm::vec3 offset) {
	this->m_offset = offset;
	this->update_model_matrix();
}

void Object::scale(glm::vec3 scale) {
	this->m_scale = scale;
	this->update_model_matrix();
}

void Object::set_scale_factor(glm::vec3 fac) {
	this->m_scale_factor = fac;
	this->update_model_matrix();
}

void Object::rotate(float deg, glm::vec3 v) {
	this->m_rotation = glm::radians(deg);
	this->m_rotation_vec = v;
	this->update_model_matrix();
}

void Object::update_model_matrix() {
	// update model pos
	this->m_model = glm::mat4(1.0f);
	this->m_model = glm::translate(this->m_model, this->get_world_position());
	this->m_model = glm::rotate(this->m_model, this->m_rotation, this->m_rotation_vec);
	this->m_model = glm::scale(this->m_model, this->m_scale * this->m_scale_factor);
	for (const auto c : this->m_children) {
		c->update_model_matrix();
	}
}

void Object::update() {
	if (this->m_shader) {
		this->m_shader->set_active();
		this->m_shader->set_model(this->m_model);
	}
	if (this->m_meshes) {
		for (auto iter = this->m_meshes->begin(); iter != this->m_meshes->end(); ++iter) {
			(*iter)->draw();
		}
	}
}

const std::shared_ptr<std::vector<std::shared_ptr<TexturedMesh>>> Object::get_textured_meshes() const {
	return this->m_meshes;
}

glm::vec3 Object::get_world_position() const {
	glm::vec3 pos = this->m_pos + this->m_offset;
	if (this->m_parent) {
		pos += this->m_parent->get_world_position();
	}
	return pos;
}

std::vector<Object*> Object::get_all_children() {
	std::vector<Object*> ret;
	std::copy(this->m_children.begin(), this->m_children.end(), std::back_inserter(ret));
	for (const auto c : this->m_children) {
		auto children_vec = c->get_all_children();
		std::copy(children_vec.begin(), children_vec.end(), std::back_inserter(ret));
	}
	return ret;
}

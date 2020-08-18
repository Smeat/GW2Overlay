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

#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GL/gl.h>

Object::Object(std::shared_ptr<Shader> s, std::vector<Mesh> vertex_data) {
	this->m_shader = s;
	this->m_meshes = vertex_data;
}
void Object::translate(glm::vec3 pos) { this->m_pos = pos; }

void Object::scale(glm::vec3 scale) { this->m_scale = scale; }

void Object::rotate(float deg, glm::vec3 v) {
	this->m_rotation = glm::radians(deg);
	this->m_rotation_vec = v;
}

void Object::update() {
	// update model pos
	this->m_model = glm::mat4(1.0f);
	this->m_model = glm::translate(this->m_model, this->m_pos);
	this->m_model =
		glm::rotate(this->m_model, this->m_rotation, this->m_rotation_vec);
	this->m_model = glm::scale(this->m_model, this->m_scale);

	this->m_shader->set_active();
	this->m_shader->set_mat4("transform", this->m_model);
	for (auto iter = this->m_meshes.begin(); iter != this->m_meshes.end();
		 ++iter) {
		iter->draw();
	}
}

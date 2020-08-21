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
#include <vector>

class Object {
 public:
	Object(std::shared_ptr<Shader> s, std::vector<std::shared_ptr<TexturedMesh>> vertex_data);
	void translate(glm::vec3 pos);
	void scale(glm::vec3 scale);
	void rotate(float deg, glm::vec3 v);
	void update();

 private:
	std::shared_ptr<Shader> m_shader;
	std::vector<std::shared_ptr<TexturedMesh>> m_meshes;
	glm::mat4 m_model;

	// position etc
	glm::vec3 m_pos = glm::vec3(0, 0, 0);
	glm::vec3 m_scale = glm::vec3(1);
	glm::vec3 m_rotation_vec = glm::vec3(0.0f, 0.0f, 1.0f);
	float m_rotation = glm::radians(180.0f);
};

#endif	// __OBJECT__H__

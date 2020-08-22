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
#ifndef __VKSHADER__H_
#define __VKSHADER__H_

#include <glm/fwd.hpp>
#include <string>

#include <glm/glm.hpp>

#include "../../Shader.h"

/**
 * To use multiple shaders, we need to create separate pipelines
 */
class VKShader : public Shader {
 public:
	VKShader(const std::string& vertex_path, const std::string& fragment_path);
	virtual ~VKShader();

	void set_active() override;
	virtual void set_projection(glm::mat4 p) override;
	virtual void set_view(glm::mat4 v) override;
	virtual void set_model(glm::mat4 m) override;

	glm::mat4 get_projection();
	glm::mat4 get_view();
	glm::mat4 get_model();

 private:
	glm::mat4 m_projection = glm::mat4(1.0f);
	glm::mat4 m_view = glm::mat4(1.0f);
	glm::mat4 m_model = glm::mat4(1.0f);
};

#endif	// __VKSHADER__H_

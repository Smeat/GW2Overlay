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
#include "VKShader.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/glm.hpp>

#include <fstream>
#include <iostream>
#include <sstream>

VKShader::VKShader(const std::string& vertex_path, const std::string& fragment_path)
	: Shader(vertex_path, fragment_path) {}

VKShader::~VKShader() {}

VKShaderMVP::VKShaderMVP(const std::string& vertex_path, const std::string& fragment_path)
	: VKShader(vertex_path, fragment_path) {}

void VKShaderMVP::set_projection(glm::mat4 projection) {
	this->m_ubo.proj = projection;
	// FIXME: fix this somewhere else
	this->m_ubo.proj[1][1] *= -1;
}
void VKShaderMVP::set_view(glm::mat4 v) { this->m_ubo.view = v; }
void VKShaderMVP::set_model(glm::mat4 m) { this->m_ubo.model = m; }

glm::mat4 VKShaderMVP::get_projection() { return this->m_ubo.proj; }
glm::mat4 VKShaderMVP::get_view() { return this->m_ubo.view; }
glm::mat4 VKShaderMVP::get_model() { return this->m_ubo.model; }

size_t VKShaderMVP::get_uniform_size() { return sizeof(this->m_ubo); }
void* VKShaderMVP::get_uniform_data() { return &this->m_ubo; }

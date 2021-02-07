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

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <memory>

#include "../../../utils/FileHelper.h"
#include "../../Shader.h"

/**
 * To use multiple shaders, we need to create separate pipelines
 */
class VKShader : public Shader {
 public:
	VKShader(const std::string& vertex_path, const std::string& fragment_path);
	virtual ~VKShader();

	void set_active() override{};
	bool operator==(const std::shared_ptr<VKShader> other);

 private:
	std::string m_vertex_shader_path;
	std::string m_fragment_shader_path;
};

class VKShaderMVP : public VKShader {
 public:
	VKShaderMVP(const std::string& vertex_path, const std::string& fragment_path);

	virtual void set_projection(glm::mat4 p) override;
	virtual void set_view(glm::mat4 v) override;
	virtual void set_model(glm::mat4 m) override;

	glm::mat4 get_projection();
	glm::mat4 get_view();
	glm::mat4 get_model();

	virtual size_t get_uniform_size() override;
	virtual void* get_uniform_data() override;

 private:
	struct {
		alignas(16) glm::mat4 model = glm::mat4(1.0f);
		alignas(16) glm::mat4 view = glm::mat4(1.0f);
		alignas(16) glm::mat4 proj = glm::mat4(1.0f);
	} m_ubo;
};

#endif	// __VKSHADER__H_

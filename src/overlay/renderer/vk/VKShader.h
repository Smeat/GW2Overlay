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

class VKShader : public Shader {
 public:
	VKShader(const std::string& vertex_path, const std::string& fragment_path);
	virtual ~VKShader();

	void set_active() override;
	virtual void set_projection(glm::mat4 p) override;
	virtual void set_view(glm::mat4 v) override;
	virtual void set_model(glm::mat4 m) override;
};

#endif	// __VKSHADER__H_

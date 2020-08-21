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
#ifndef __SHADER__H_
#define __SHADER__H_

#include <glm/fwd.hpp>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Shader {
 public:
	virtual void set_active() = 0;
	virtual void set_projection(glm::mat4 p) = 0;
	virtual void set_view(glm::mat4 v) = 0;
	virtual void set_model(glm::mat4 m) = 0;

	virtual void set_projection(float fov_rad, float w, float h, float near = 0.1f, float far = 1000.0f) {
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspectiveFovLH(fov_rad, w, h, near, far);
		this->set_projection(projection);
	}
	virtual void load_from_string(const std::string& vert, const std::string& frag){};
};

#endif	// __SHADER__H_

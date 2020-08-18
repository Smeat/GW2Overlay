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

#include <string>

#include <glm/glm.hpp>

class Shader {
 public:
	Shader() = default;
	Shader(const std::string& vertex_path, const std::string& fragment_path);
	virtual ~Shader();
	void load_from_string(const std::string& vertex_src,
						  const std::string& fragment_src);
	void set_active();
	void set_mat4(const std::string& name, const glm::mat4& mat);

 private:
	int m_program_id = 0;
};

#endif	// __SHADER__H_

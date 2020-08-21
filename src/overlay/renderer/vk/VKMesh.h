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
#ifndef __VKMESH_H__
#define __VKMESH_H__

#include <vulkan/vulkan.h>
#include <glm/fwd.hpp>
#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "../../Mesh.h"

class VKMesh : public Mesh {
 public:
	VKMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
	virtual ~VKMesh();
	virtual void draw() override;

 private:
	unsigned int m_vao, m_vbo, m_ebo;
};

#endif

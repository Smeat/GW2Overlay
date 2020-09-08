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
#include "VKObject.h"
#include "../../Mesh.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <memory>
#include <stdexcept>

#include <GL/gl.h>
#include <vulkan/vulkan_core.h>

#include "VKCommon.h"
#include "VKTexture.h"

// TODO: Is it better to have one large buffer and adjust it, when the objects change?
VKObject::VKObject(std::shared_ptr<Shader> s, std::vector<std::shared_ptr<TexturedMesh>> vertex_data)
	: Object(s, vertex_data) {}

VKObject::~VKObject() {}

std::vector<VkDescriptorSet>* VKObject::get_descriptor_sets() { return &this->m_descriptor_sets; }

void VKObject::set_descriptor_sets(int pos, const VkDescriptorSet& set) {
	if ((pos + 1) > this->m_descriptor_sets.size()) {
		this->m_descriptor_sets.resize(pos + 1);
	}
	this->m_descriptor_sets[pos] = set;
}

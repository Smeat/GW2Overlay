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
#ifndef __VKOBJECT__H__
#define __VKOBJECT__H__

#include <vulkan/vulkan_core.h>
#include <glm/fwd.hpp>
#include "../../Mesh.h"
#include "../../Object.h"
#include "../../Shader.h"

#include <glm/trigonometric.hpp>
#include <memory>
#include <vector>

class VKObject : public Object {
 public:
	VKObject(std::shared_ptr<Shader> s, std::vector<std::shared_ptr<TexturedMesh>> vertex_data);
	virtual ~VKObject();
	const std::shared_ptr<std::vector<std::shared_ptr<TexturedMesh>>> get_textured_meshes() const;
	const std::shared_ptr<Shader> get_shader() const { return this->m_shader; }
	std::vector<VkDescriptorSet>* get_descriptor_sets();
	void set_descriptor_sets(int pos, const VkDescriptorSet& set);

 private:
	VkDevice m_device;
	std::vector<VkDescriptorSet> m_descriptor_sets;
};

#endif	// __VKOBJECT__H__

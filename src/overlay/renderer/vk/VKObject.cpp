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
VKObject::VKObject(std::shared_ptr<Shader> s, std::vector<std::shared_ptr<TexturedMesh>> vertex_data, VkDevice& device,
				   VkPhysicalDevice& physicalDevice, VkDescriptorSetLayout& layout, uint32_t images)
	: Object(s, vertex_data) {
	this->m_device = device;

	std::array<VkDescriptorPoolSize, 2> poolSizes{};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = static_cast<uint32_t>(images);
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = static_cast<uint32_t>(images);

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(images);

	if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &this->m_descriptor_pool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
	std::vector<VkDescriptorSetLayout> layouts(images, layout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = this->m_descriptor_pool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(images);
	allocInfo.pSetLayouts = layouts.data();
	this->m_descriptor_sets.resize(images);
	VkResult result;
	if ((result = vkAllocateDescriptorSets(device, &allocInfo, this->m_descriptor_sets.data())) != VK_SUCCESS) {
		std::cerr << "Result is " << result << std::endl;
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	VkDeviceSize bufferSize = sizeof(UniformBufferObject);
	this->m_uniform_buffers.resize(images);
	this->m_uniform_buffers_memory.resize(images);

	for (size_t i = 0; i < images; ++i) {
		createBuffer(device, physicalDevice, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					 this->m_uniform_buffers[i], this->m_uniform_buffers_memory[i]);

		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = this->m_uniform_buffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		// TODO: move texture to object?
		auto tex = std::dynamic_pointer_cast<VKTexture>(this->m_meshes->at(0)->get_texture());
		imageInfo.imageView = tex->get_image_view();
		imageInfo.sampler = tex->get_sampler();

		std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = this->m_descriptor_sets[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = this->m_descriptor_sets[i];
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0,
							   nullptr);
	}
}

VKObject::~VKObject() {
	for (int i = 0; i < this->m_uniform_buffers.size(); ++i) {
		vkDestroyBuffer(this->m_device, this->m_uniform_buffers[i], nullptr);
		vkFreeMemory(this->m_device, this->m_uniform_buffers_memory[i], nullptr);
	}
	vkDestroyDescriptorPool(this->m_device, this->m_descriptor_pool, nullptr);
}

std::vector<VkDescriptorSet>* VKObject::get_descriptor_sets() { return &this->m_descriptor_sets; }

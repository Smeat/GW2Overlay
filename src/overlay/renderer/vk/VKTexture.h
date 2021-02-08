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
#ifndef __VKTEXTURE_H__
#define __VKTEXTURE_H__

#include <vulkan/vulkan_core.h>

#include <string>

#include "../../Texture.h"

class VKTexture : public Texture {
 public:
	VKTexture(VkDevice device, VkPhysicalDevice physical_device, VkCommandPool command_pool, VkQueue graphics_queue);
	VKTexture(const std::string& path, VkDevice device, VkPhysicalDevice physical_device, VkCommandPool command_pool,
			  VkQueue graphics_queue);
	VKTexture(SDL_Surface* surf, VkDevice device, VkPhysicalDevice physical_device, VkCommandPool command_pool,
			  VkQueue graphics_queue);
	virtual ~VKTexture();

	virtual void set_active() override;

	VkImageView get_image_view() { return this->m_texture_image_view; }
	VkSampler get_sampler() { return this->m_texture_sampler; }
	void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
					 VkMemoryPropertyFlags properties);
	void createTextureImageView(VkFormat format = VK_FORMAT_R8G8B8A8_SRGB,
								VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT);

 private:
	void createTextureSampler();
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void createTextureImage(SDL_Surface* surf);
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

	VkDevice m_device;
	VkPhysicalDevice m_physical_device;
	VkCommandPool m_command_pool;
	VkQueue m_graphics_queue;

	VkImage m_texture_image;
	VkDeviceMemory m_texture_image_memory;
	VkImageView m_texture_image_view;
	VkSampler m_texture_sampler;
};

#endif

#ifndef __VKCOMMON_H__
#define __VKCOMMON_H__

#include <vulkan/vulkan_core.h>

void createBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage,
				  VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
void endSingleTimeCommands(VkDevice device, VkCommandPool commandPool, VkCommandBuffer commandBuffer,
						   VkQueue graphicsQueue);
VkCommandBuffer beginSingleTimeCommands(VkDevice device, VkCommandPool commandPool);
VkImageView createImageView(VkDevice device, VkImage image, VkFormat format,
							VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT);

#endif

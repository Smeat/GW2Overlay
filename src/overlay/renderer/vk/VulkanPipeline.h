#ifndef __VULKANPIPELINE_H__
#define __VULKANPIPELINE_H__

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <memory>

#include "VKShader.h"

struct VulkanPipelineSettings {
	std::shared_ptr<VKShader> shader;
	VkDevice device;
	VkExtent2D swapchain_extend;
	VkPipelineLayout pipeline_layout;
	VkRenderPass render_pass;

	// TODO: other options
	bool operator==(const VulkanPipelineSettings& other) const;
};

class VulkanPipeline {
 public:
	VulkanPipeline(VulkanPipelineSettings settings);
	virtual ~VulkanPipeline();

	VkPipeline get_pipeline() const { return this->m_graphics_pipeline; }
	VulkanPipelineSettings get_settings() const { return this->m_pipeline_settings; }
	bool operator==(const VulkanPipeline& other) const;

 private:
	void create_pipeline(VulkanPipelineSettings pipeline_settings);
	VkPipeline m_graphics_pipeline;
	VulkanPipelineSettings m_pipeline_settings;
};

namespace std {
template <>
struct hash<VulkanPipelineSettings> {
	size_t operator()(const VulkanPipelineSettings& settings) const { return 1; }
};

}  // namespace std

#endif	//  __VULKANPIPELINE_H__

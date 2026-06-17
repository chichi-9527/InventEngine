#ifndef _IVULKANRENDERER_
#define _IVULKANRENDERER_

#ifdef USE_VULKAN


#include <vulkan/vulkan.h>
#include <vector>

namespace INVENT
{

	class ICamera;

	class IVulkanRenderer
	{
	public:
		static bool Init();
		static void Shutdown();

		static void BeginRender(const ICamera* camera);
		static void EndRender();

		static bool WaitForFence(uint32_t frame_index);
		static int AcquireNextImage(uint32_t frame_index);
		static bool ToRenderScence(uint32_t frame_index);

	private:
		static bool _init_pipelines();
		static bool _create_sync_objects();
		static bool _create_commnad_buffers();
		static void _rendering_shadow_opaque();

	private:
		// shadow
		inline static VkPipeline _shadow_pipeline_opaque = VK_NULL_HANDLE;
		inline static VkPipeline _shadow_pipeline_masked = VK_NULL_HANDLE;
		// offscreen
		inline static VkPipeline _offscreen_opaque_pipeline = VK_NULL_HANDLE;
		inline static VkPipeline _offscreen_masked_pipeline = VK_NULL_HANDLE;
		inline static VkPipeline _offscreen_translucent_pipeline = VK_NULL_HANDLE;
		// main
		inline static VkPipeline _main_opaque_pipeline = VK_NULL_HANDLE;
		inline static VkPipeline _main_masked_pipeline = VK_NULL_HANDLE;
		inline static VkPipeline _main_translucent_pipeline = VK_NULL_HANDLE;
		// postprocess
		inline static VkPipeline _postprocess_pipeline = VK_NULL_HANDLE;
		// ui
		inline static VkPipeline _ui_pipeline = VK_NULL_HANDLE;

		inline static std::vector<VkFence> _frame_fences;
		inline static std::vector<VkSemaphore> _acquire_semaphores;
		inline static std::vector<VkSemaphore> _submit_semaphores;
		inline static std::vector<VkCommandBuffer> _command_buffers;

		
	};


}

#endif // USE_VULKAN

#endif // !_IVULKANRENDERER_
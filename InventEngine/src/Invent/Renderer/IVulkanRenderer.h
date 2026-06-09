#ifndef _IVULKANRENDERER_
#define _IVULKANRENDERER_

#include <vulkan/vulkan.h>

namespace INVENT
{

	class IVulkanRenderer
	{
	public:
		static void Init();
		static void Shutdown();



	private:
		static bool InitPipelines();
		

	private:
		// shadow
		inline static VkPipeline _shadow_pipeline = VK_NULL_HANDLE;
		// offscreen
		inline static VkPipeline _offscreen_opaque_pipeline = VK_NULL_HANDLE;
	};


}

#endif // !_IVULKANRENDERER_
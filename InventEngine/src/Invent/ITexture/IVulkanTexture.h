#ifndef _IVULKANTEXTURE_
#define _IVULKANTEXTURE_

#include <vulkan/vulkan.h>

namespace INVENT
{
	struct IVulkanTexture2D
	{
		VkImageView ImageView = VK_NULL_HANDLE;
		uint32_t SlotId = 0;

	};


	class IVulkanTexture2DManagement
	{
		IVulkanTexture2DManagement() = default;
	public:
		static IVulkanTexture2DManagement& Instance();



	private:


	};

}

#endif // !_IVULKANTEXTURE_
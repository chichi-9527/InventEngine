#include "IEpch.h"
#include "IVulkanTexture.h"

namespace INVENT
{


	IVulkanTexture2DManagement& IVulkanTexture2DManagement::Instance()
	{
		static IVulkanTexture2DManagement m;
		return m;
	}
}

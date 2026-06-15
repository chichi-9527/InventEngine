#ifndef _VULKANGLOBALMATERIALMANAGER_
#define _VULKANGLOBALMATERIALMANAGER_

#ifdef USE_VULKAN

#include "Invent/ITools/IBitArray.h"

namespace INVENT
{

	// 最大材质数量，物理设备极限计算：VulkanBase::_physical_device_properties.limits.maxStorageBufferRange / sizeof(MeshMaterialData)
	constexpr uint32_t MAX_MATERIAL_COUNT = 200000;

	struct IMaterialData
	{

	};


	class VulkanGlobalMaterialManager
	{
		VulkanGlobalMaterialManager() = default;
	public:
		~VulkanGlobalMaterialManager();



	private:

	};
}



#endif // USE_VULKAN

#endif // !_VULKANGLOBALMATERIALMANAGER_




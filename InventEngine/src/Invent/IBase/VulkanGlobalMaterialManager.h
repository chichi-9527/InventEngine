#ifndef _VULKANGLOBALMATERIALMANAGER_
#define _VULKANGLOBALMATERIALMANAGER_

#ifdef USE_VULKAN

#include "Invent/ITools/IBitArray.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <utility>
#include <cstdint>

namespace INVENT
{

	// 最大材质数量，物理设备极限计算：VulkanBase::_physical_device_properties.limits.maxStorageBufferRange / sizeof(MeshMaterialData)
	constexpr uint32_t MAX_MATERIAL_COUNT = 200000;

	// 32 bit
	enum IMaterialTextureReadyFlags : std::uint32_t
	{
		ReadyBitAllNotReady = 0,
		ReadyBitDeffuse = 1 << 0,
		ReadyBitNormal = 1 << 1,
		ReadyBitSpecular = 1 << 2,
		ReadyBitEmission = 1 << 3,
		ReadyBitAo = 1 << 4,
		ReadyBitOpacity = 1 << 5,
		ReadyBitRoughness = 1 << 6,
		ReadyBitClearCoat = 1 << 7
	};

	enum IMaterialAttributeFlag : std::uint32_t
	{
		UnDefined = 0,
		Metal = 1,
		NonMetal = 2,
		Water = 3
	};

	// 32 位对齐
	struct IMaterialData
	{
		// 
		uint32_t DiffuseTextureId = 0;
		// 法线贴图
		uint32_t NormalTextureId = 0;
		// 镜面反射贴图
		uint32_t SpecularTextureId = 0;
		// 自发光
		uint32_t EmissionTextureId = 0;
		// 环境遮挡
		uint32_t AoTextureId = 0;
		// 不透明度
		uint32_t OpacityTextureId = 0;
		// 粗糙度
		uint32_t RoughnessTextureId = 0;
		// 透明涂层
		uint32_t ClearCoatTextureId = 0;

		// 材质属性 IMaterialAttributeFlag
		uint32_t MaterialAttribute = IMaterialAttributeFlag::UnDefined;
		// 贴图标记（是否准备好）
		uint32_t textureReadyFlags = IMaterialTextureReadyFlags::ReadyBitAllNotReady;

		// 
		uint32_t padding[6] = {};
	};


	class VulkanGlobalMaterialManager
	{
		VulkanGlobalMaterialManager();
	public:
		~VulkanGlobalMaterialManager();

		static VulkanGlobalMaterialManager& Instance();

		using MaterialHandle = IHandle;

		MaterialHandle AlocateHandle();
		MaterialHandle AddMaterial(const IMaterialData& data);
		void UpdateMaterial(const MaterialHandle& handle, const IMaterialData& data);
		void DestroyMaterial(const MaterialHandle& handle);

		bool IsUsed(const MaterialHandle& handle) const;

		void SyncToGPU();

		bool IsVaild() const { return _data != nullptr; }

	private:
		void* _data = nullptr;

		VkBuffer _ssbo_material = VK_NULL_HANDLE;
		IBitVector _bit_vector_dirty;
		IBitVector _bit_vector_used;
		std::vector<IMaterialData> _materials;

		uint32_t _current_material_count = 0;

	};
}



#endif // USE_VULKAN

#endif // !_VULKANGLOBALMATERIALMANAGER_




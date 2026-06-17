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
		ReadyBitDeffuse = 1 << 0,
		ReadyBitNormal = 1 << 1
	};

	// 32 位对齐
	struct IMaterialData
	{
		uint32_t diffuseTextureId = 0;
		uint32_t normalTextureId = 0;
		uint32_t specularTextureId = 0;
		uint32_t emissionTextureId = 0;

		uint32_t shadowMapTextureId = 0;
		uint32_t blendMode = 0;

		uint32_t textureReadyFlags = 0;
		uint32_t padding = 0;
	};


	class VulkanGlobalMaterialManager
	{
		VulkanGlobalMaterialManager();
	public:
		~VulkanGlobalMaterialManager();

		static VulkanGlobalMaterialManager& Instance();

		struct MaterialHandle
		{
			size_t BitSetIndex = MaxSizeTValue;
			// BitSet 中的索引(0~63)
			size_t BitIndex = 64;

			MaterialHandle() = default;
			MaterialHandle(const std::pair<size_t, size_t>& v)
				: BitSetIndex(v.first)
				, BitIndex(v.second)
			{}
			MaterialHandle(const MaterialHandle&) = default;
			MaterialHandle(MaterialHandle&&) noexcept = default;

			MaterialHandle& operator=(const std::pair<size_t, size_t>& v)
			{
				BitSetIndex = v.first;
				BitIndex = v.second;
				return *this;
			}
			MaterialHandle& operator=(const MaterialHandle&) = default;
			MaterialHandle& operator=(MaterialHandle&&) noexcept = default;

			friend bool operator==(const MaterialHandle& handle, const std::pair<size_t, size_t>& v)
			{
				return handle.BitSetIndex == v.first &&
					handle.BitIndex == v.second;
			}
			friend bool operator==(const MaterialHandle& handle1, const MaterialHandle& handle2)
			{
				return handle1.BitSetIndex == handle2.BitSetIndex &&
					handle1.BitIndex == handle2.BitIndex;
			}

			size_t GetMaterialIndex() const noexcept
			{
				return BitSetIndex * 64 + BitIndex;
			}

			bool IsVaild() const noexcept
			{
				return BitSetIndex != MaxSizeTValue &&
					BitIndex < 64;
			}
		};

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




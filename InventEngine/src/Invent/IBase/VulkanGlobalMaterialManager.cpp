#include "IEpch.h"
#include "VulkanGlobalMaterialManager.h"

#ifdef USE_VULKAN

#include "Invent/IBase/VulkanBase.h"


namespace INVENT
{
	VulkanGlobalMaterialManager::VulkanGlobalMaterialManager()
	{
		_current_material_count = std::min(MAX_MATERIAL_COUNT, 
			VulkanBase::Base().GetPhysicalDeviceProperties().limits.maxStorageBufferRange / static_cast<uint32_t>(sizeof(IMaterialData)));
		INVENT_LOG_INFO(std::format("[ VulkanGlobalMaterialManager ] current material count : {}. \n", _current_material_count));

		if (!VulkanBase::Base().UseVmaCreateBuffer(std::uint64_t{_current_material_count}*sizeof(IMaterialData), 
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			_ssbo_material))
		{
			return;
		}

		if (!VulkanBase::Base().UseVmaMapMemory(_ssbo_material, _data))
		{
			_data = nullptr;
			return;
		}

		_bit_vector_dirty.ResizeBitCount(_current_material_count);
		_bit_vector_used.ResizeBitCount(_current_material_count);
		_materials.resize(_current_material_count);

	}

	VulkanGlobalMaterialManager::~VulkanGlobalMaterialManager()
	{
		if (_ssbo_material != VK_NULL_HANDLE)
		{
			if (_data != nullptr)
			{
				VulkanBase::Base().UseVmaUnmapMemory(_ssbo_material);
				_data = nullptr;
			}
			VulkanBase::Base().UseVmaDestroyBuffer(_ssbo_material);
		}
			
	}

	VulkanGlobalMaterialManager& VulkanGlobalMaterialManager::Instance()
	{
		static VulkanGlobalMaterialManager m;
		return m;
	}

	VulkanGlobalMaterialManager::MaterialHandle VulkanGlobalMaterialManager::AlocateHandle()
	{
		MaterialHandle handle = _bit_vector_used.FindFirstZero();
		if (!handle.IsVaild())
			return MaterialHandle();
		_bit_vector_dirty.SetValue<true>(handle.BitSetIndex, handle.BitIndex);
		_bit_vector_used.SetValue<true>(handle.BitSetIndex, handle.BitIndex);
		return handle;
	}

	VulkanGlobalMaterialManager::MaterialHandle VulkanGlobalMaterialManager::AddMaterial(const IMaterialData& data)
	{
		MaterialHandle handle = _bit_vector_used.FindFirstZero();
		if (!handle.IsVaild())
			return MaterialHandle();
		_materials[handle.GetRealIndex()] = data;
		_bit_vector_dirty.SetValue<true>(handle.BitSetIndex, handle.BitIndex);
		_bit_vector_used.SetValue<true>(handle.BitSetIndex, handle.BitIndex);
		return handle;
	}

	void VulkanGlobalMaterialManager::UpdateMaterial(const MaterialHandle& handle, const IMaterialData& data)
	{
		if (!handle.IsVaild() ||
			handle.GetRealIndex() >= static_cast<size_t>(_current_material_count))
			return;
		_materials[handle.GetRealIndex()] = data;
		_bit_vector_dirty.SetValue<true>(handle.BitSetIndex, handle.BitIndex);
	}

	void VulkanGlobalMaterialManager::DestroyMaterial(const MaterialHandle& handle)
	{
		if (!handle.IsVaild() ||
			handle.GetRealIndex() >= static_cast<size_t>(_current_material_count))
			return;
		_bit_vector_used.SetValue<false>(handle.BitSetIndex, handle.BitIndex);
	}

	bool VulkanGlobalMaterialManager::IsUsed(const MaterialHandle& handle) const
	{
		if (!handle.IsVaild() ||
			handle.GetRealIndex() >= static_cast<size_t>(_current_material_count))
			return false;
		return _bit_vector_used[handle.BitSetIndex][handle.BitIndex];
	}

	void VulkanGlobalMaterialManager::SyncToGPU()
	{
		if (!IsVaild())return;

		_bit_vector_dirty.ForEach([this](size_t index, bool is_dirty) {
			if (is_dirty && index < _materials.size())
			{
				size_t memOffset = index * sizeof(IMaterialData);
				void* targetAddress = reinterpret_cast<char*>(_data) + memOffset;
				std::memcpy(targetAddress, &_materials[index], sizeof(IMaterialData));

				size_t bitSetIdx = index / 64;
				size_t bitIdx = index % 64;
				_bit_vector_dirty.SetValue<false>(bitSetIdx, bitIdx);
			}
			});

	}


}



#endif // USE_VULKAN

#include "IEpch.h"
#include "IVulkanTexture.h"

#ifdef USE_VULKAN

#include "Invent/IEngineTools.h"
#include "Invent/ThreadPool/IThreadPool.h"
#include "Invent/IMemPool/IMemPool.h"
#include "StbImage/stb_image.h"
#include "Invent/IBase/VulkanBase.h"

#include <unordered_map>
#include <stdexcept>

namespace INVENT
{

	using TextureNameMap = std::unordered_map < std::string,
		IHandle,
		std::hash<std::string>,
		std::equal_to<std::string>,
		IMemPoolAllocatorOnlyFixedBlock<std::pair<const std::string, IHandle>>>;

	static TextureNameMap TextureNameCache(64,
		std::hash<std::string>(),
		std::equal_to<std::string>(),
		IMemPoolAllocatorOnlyFixedBlock< std::pair<const std::string, IHandle>>(IEngineTools::Instance().GetMemPoolPool()));

	IVulkanTexture2DManagement::IVulkanTexture2DManagement()
	{
		auto textureCount = static_cast<size_t>(VulkanBase::Base().GetCurrentBindlessDescriptorCount());

		_textures.resize(textureCount, IVulkanTexture2DHandle());
		_bit_vector_used.ResizeBitCount(textureCount);
		_bit_vector_vaild.ResizeBitCount(textureCount);

		_init_default_image();
		_is_vaild = true;
	}

	IVulkanTexture2DManagement::~IVulkanTexture2DManagement()
	{}

	IVulkanTexture2DManagement& IVulkanTexture2DManagement::Instance()
	{
		static IVulkanTexture2DManagement m;
		return m;
	}

	void IVulkanTexture2DManagement::Clear()
	{
		_bit_vector_used.ForEach([this](size_t index, bool bit_value) {
			if (bit_value)
			{
				auto& tex = _textures[index];
				if (tex.ImageView != VK_NULL_HANDLE)
				{
					VulkanBase::Base().DestroyImageView(tex.ImageView);
				}
				if (tex.Image != VK_NULL_HANDLE)
				{
					VulkanBase::Base().UseVmaDestroyImage(tex.Image);
				}
			}
			});
	}

	IVulkanTexture2DManagement::Texture2DHandle IVulkanTexture2DManagement::AllocateTextureHandle()
	{
		Texture2DHandle handle = _bit_vector_used.FindFirstZero();
		if (!handle.IsVaild())
		{
			int result = VulkanBase::Base().ResizeBindlessDescriptorPoolAndGobalSet();
			if (result == 0)
			{
				_update_texture_count();

				handle = _bit_vector_used.FindFirstZero();
			}
			else
			{
				throw std::runtime_error(std::format("ResizeBindlessDescriptorPoolAndGobalSet , error code: {}", result));
				return Texture2DHandle();
			}
			
		}

		//auto index = handle.GetRealIndex();

		_bit_vector_used.SetValue<true>(handle.BitSetIndex, handle.BitIndex);
		
		/*
		_bit_vector_vaild.SetValue<false>(handle.BitSetIndex, handle.BitIndex);
		_textures[index] = IVulkanTexture2DHandle{ VK_NULL_HANDLE, VK_NULL_HANDLE };
		*/
		
		return handle;
	}

	IVulkanTexture2DManagement::Texture2DHandle IVulkanTexture2DManagement::AddTexture2D(const std::string& path)
	{
		auto handle = AllocateTextureHandle();

		IEngineTools::Instance().GetWorkThreadPool()->Submit(0, [this, &handle, &path]() {

			});
		
		return Texture2DHandle();
	}

	void IVulkanTexture2DManagement::_update_texture_count()
	{
		auto textureCount = static_cast<size_t>(VulkanBase::Base().GetCurrentBindlessDescriptorCount());
		_bit_vector_used.ResizeBitCount(textureCount);
		_bit_vector_vaild.ResizeBitCount(textureCount);
		_textures.resize(textureCount, IVulkanTexture2DHandle());
	}

}

#endif // USE_VULKAN
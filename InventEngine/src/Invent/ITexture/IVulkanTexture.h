#ifndef _IVULKANTEXTURE_
#define _IVULKANTEXTURE_

#include "Invent/ITools/IBitArray.h"
#include <vulkan/vulkan.h>

namespace INVENT
{
	struct IVulkanTexture2DHandle
	{
		VkImageView ImageView = VK_NULL_HANDLE;
		VkImage Image = VK_NULL_HANDLE;
		size_t BitSetIndex = MaxSizeTValue;
		size_t BitIndex = 64;

		IVulkanTexture2DHandle() = default;
		IVulkanTexture2DHandle(const std::pair<size_t, size_t>& v)
			: BitSetIndex(v.first)
			, BitIndex(v.second)
		{}
		IVulkanTexture2DHandle(const std::pair<size_t, size_t>& v, VkImageView image_view, VkImage image)
			: BitSetIndex(v.first)
			, BitIndex(v.second)
			, ImageView(image_view)
			, Image(image)
		{}
		IVulkanTexture2DHandle(const IVulkanTexture2DHandle&) = default;
		IVulkanTexture2DHandle(IVulkanTexture2DHandle&&) noexcept = default;

		IVulkanTexture2DHandle& operator=(const std::pair<size_t, size_t>& v)
		{
			BitSetIndex = v.first;
			BitIndex = v.second;
			ImageView = VK_NULL_HANDLE;
			Image = VK_NULL_HANDLE;
			return *this;
		}
		IVulkanTexture2DHandle& operator=(const IVulkanTexture2DHandle&) = default;
		IVulkanTexture2DHandle& operator=(IVulkanTexture2DHandle&&) noexcept = default;

		friend bool operator==(const IVulkanTexture2DHandle& handle, const std::pair<size_t, size_t>& v)
		{
			return handle.BitSetIndex == v.first &&
				handle.BitIndex == v.second;
		}
		friend bool operator==(const IVulkanTexture2DHandle& handle1, const IVulkanTexture2DHandle& handle2)
		{
			return handle1.BitSetIndex == handle2.BitSetIndex &&
				handle1.BitIndex == handle2.BitIndex &&
				handle1.Image == handle2.Image &&
				handle1.ImageView == handle2.ImageView;
		}

		size_t GetTextureIndex() const noexcept
		{
			return BitSetIndex * 64 + BitIndex;
		}

		bool IsVaild() const noexcept
		{
			return BitSetIndex != MaxSizeTValue &&
				BitIndex < 64;
		}
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
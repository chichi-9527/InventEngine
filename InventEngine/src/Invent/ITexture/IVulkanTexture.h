#ifndef _IVULKANTEXTURE_
#define _IVULKANTEXTURE_

#ifdef USE_VULKAN

#include "Invent/ITools/IBitArray.h"
#include <vulkan/vulkan.h>

#include <string>

namespace INVENT
{
	
	class IVulkanTexture2DManagement
	{
		struct IVulkanTexture2DHandle
		{
			VkImageView ImageView = VK_NULL_HANDLE;
			VkImage Image = VK_NULL_HANDLE;
			
			bool IsVaild() const noexcept
			{
				return Image != VK_NULL_HANDLE &&
					ImageView != VK_NULL_HANDLE;
			}
		};


		IVulkanTexture2DManagement();
	public:
		~IVulkanTexture2DManagement();

		using Texture2DHandle = IHandle;

		static IVulkanTexture2DManagement& Instance();
		void Clear();

		Texture2DHandle AllocateTextureHandle();
		Texture2DHandle AddTexture2D(const std::string& path);
		void UpateTexture2D(const Texture2DHandle& hanlde, const std::string& path);
		Texture2DHandle AddTexture2D(VkImage image, VkImageView image_view);
		// 自动销毁 VkImage 与 VkImageView
		void UpateTexture2D(const Texture2DHandle& hanlde, VkImage image, VkImageView image_view);
		// 不自动销毁 VkImage 与 VkImageView
		void UpateTexture2DWithoutDestory(const Texture2DHandle& hanlde, VkImage image, VkImageView image_view);

		bool IsTextureReady(const Texture2DHandle& handle) const;
		bool IsVaild() const { return _is_vaild; }
		
		const IVulkanTexture2DHandle& GetVulkanTextureHanlde(const Texture2DHandle& handle) const;
		IVulkanTexture2DHandle& GetVulkanTextureHanlde(const Texture2DHandle& handle);

	private:
		void _init_default_image();
		
		void _update_texture_count();

	private:
		std::vector<IVulkanTexture2DHandle> _textures;
		IBitVector _bit_vector_used;
		IBitVector _bit_vector_vaild;

		bool _is_vaild = false;

	};

}

#endif // USE_VULKAN

#endif // !_IVULKANTEXTURE_
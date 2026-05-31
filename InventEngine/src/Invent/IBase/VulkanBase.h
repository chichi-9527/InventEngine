#ifndef _VULKANBASE_
#define _VULKANBASE_

#include <vulkan/vulkan.hpp>

#include <vector>

namespace INVENT
{
	class VulkanBase
	{

		struct SwapChainSupportDetails {
			VkSurfaceCapabilitiesKHR Capabilities;
			std::vector<VkSurfaceFormatKHR> Formats;
			std::vector<VkPresentModeKHR> PresentModes;
		};

		struct QueueFamilyIndices {
			uint32_t GraphicsFamily = 0;
			uint32_t PresentFamily = 0;

			bool HasGraphicsFamily = false;
			bool HasPresentFamily = false;

			bool IsComplete() const
			{
				return HasGraphicsFamily && HasPresentFamily;
			}
		};

		VulkanBase() = default;
	public:
		~VulkanBase();

		void AddValidationLayer(const char* layerName);
		void AddInstanceExtension(const char* extensionName);
		void AddDeviceExtension(const char* extensionName);

		VkInstance GetVkInstance() const { return _instance; }
		void SetSurface(VkSurfaceKHR surface) { if (!_surface) _surface = surface; }

		static VulkanBase& Base();

		bool CreateVulkanInstance();
		void CleanUp();
		bool PickPhysicalDevice();
		bool CreateLogicalDevice();
		bool CreateSwapChain();
		bool CreateSwapChainImageView();




	private:
		void _use_lastest_api_version();
		QueueFamilyIndices _find_queue_families(VkPhysicalDevice device);
		SwapChainSupportDetails _query_swap_chain_support(VkPhysicalDevice device);
		bool _check_device_extension_support(VkPhysicalDevice device);
		VkSurfaceFormatKHR _choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& available_formats);
		VkPresentModeKHR _choose_swap_presenta_mode(const std::vector<VkPresentModeKHR>& available_present_modes, VkPresentModeKHR mode = VK_PRESENT_MODE_MAILBOX_KHR);
		VkExtent2D _choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities);
		VkImageView _create_image_view(VkImage image, 
			VkFormat format, 
			VkImageAspectFlags aspect_flags, 
			VkImageViewType view_type = VK_IMAGE_VIEW_TYPE_2D, 
			uint32_t mip_level = 1,
			uint32_t base_array_layer = 0,
			uint32_t layer_count = 1);


#ifndef NDEBUG
		bool _check_validation_layers();
		bool _setup_debug_messenger();
#endif // !NDEBUG


	private:
		VkInstance _instance = VK_NULL_HANDLE;
		VkDevice _device = VK_NULL_HANDLE;
		VkSurfaceKHR _surface = VK_NULL_HANDLE;
		VkPhysicalDevice _physical_device = VK_NULL_HANDLE;
		VkQueue _graphics_queue = VK_NULL_HANDLE;
		VkQueue _present_queue = VK_NULL_HANDLE;
		VkSwapchainKHR _swap_chain = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT _debug_messenger = VK_NULL_HANDLE;

		std::vector<VkImage> _swap_chain_images;
		std::vector<VkImageView> _swap_chain_image_views;

		SwapChainSupportDetails _swap_chain_support;
		QueueFamilyIndices _queue_family_indices;
		VkExtent2D _swap_chain_extent;

		VkFormat _swap_chain_image_format = VK_FORMAT_UNDEFINED;
		VkImageUsageFlags _swap_chain_image_usages = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		uint32_t _api_version = VK_API_VERSION_1_0;
		uint32_t _frame_buffer_width = 0;
		uint32_t _frame_buffer_height = 0;
		uint32_t _swap_chain_image_count = 0;
		

	};

}

#endif // !_VULKANBASE_



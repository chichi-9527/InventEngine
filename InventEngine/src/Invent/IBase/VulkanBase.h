#ifndef _VULKANBASE_
#define _VULKANBASE_

#include <vulkan/vulkan.hpp>

#include <vector>
#include <array>
#include <functional>

namespace INVENT
{
	constexpr unsigned int MAX_FRAMES_IN_FLIGHT = 2;

	class VulkanBase
	{

		struct SwapChainSupportDetails {
			VkSurfaceCapabilitiesKHR Capabilities = {};
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

		struct OffscreenPassResources {
			VkImage Image = VK_NULL_HANDLE;
			VkImageView View = VK_NULL_HANDLE;

			// 离屏渲染关卡
			VkImageView DepthView = VK_NULL_HANDLE;
			void* Level = nullptr;

			// vulkan < 1.3
			VkFramebuffer Framebuffer = VK_NULL_HANDLE;
			VkRenderPass RenderPass = VK_NULL_HANDLE;

			// vulkan >= 1.3
			VkFormat DynamicFormat = VK_FORMAT_UNDEFINED;
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
		bool CreateVmaAllocator();
		void DestroyVmaAllocator();
		bool GetDepthFormat();
		// if vulkan < 1.3
		bool CreateDefaultRenderPasses();
		bool CreateDefaultFramebuffers();
		// all
		bool InitializeAllOffscreenPasses();
		

		// tools

		bool RecreateResizableResources();
		// this func has RecreateResizableResources()
		bool RecreateSwapChain();

		// vulkan < 1.3 tools

		struct Subpass 
		{
			std::vector<VkAttachmentReference> ColorRefs;
			std::vector<VkAttachmentReference> InputRefs;
			VkAttachmentReference DepthStencilRef = { VK_ATTACHMENT_UNUSED, VK_IMAGE_LAYOUT_UNDEFINED };
			std::vector<uint32_t> PreserveAttachment;
		};
		VkRenderPass CreateRenderPass(std::vector<VkAttachmentDescription>& attachments, 
			std::vector<Subpass>& subpasses, 
			std::vector<VkSubpassDependency>& dependencies);
		VkFramebuffer CreateFramebuffer(uint32_t width,
			uint32_t height,
			VkRenderPass render_pass, 
			uint32_t attachment_size, 
			VkImageView* attachments,
			uint32_t layers = 1);
		

		// use vma

		bool UseVmaCreateBuffer(VkDeviceSize size,
			VkBufferUsageFlags usage,
			VkMemoryPropertyFlags properties,
			VkBuffer& buffer);
		void UseVmaDestroyBuffer(VkBuffer buffer);
		bool UseVmaCreateImage(uint32_t width,
			uint32_t height,
			uint32_t mip_levels,
			VkFormat format,
			VkImageTiling tiling,
			VkImageUsageFlags usage,
			VkMemoryPropertyFlags properties,
			VkImage& image);
		void UseVmaDestroyImage(VkImage image);
		VkImageView CreateImageView(VkImage image,
			VkFormat format,
			VkImageAspectFlags aspect_flags,
			VkImageViewType view_type = VK_IMAGE_VIEW_TYPE_2D,
			uint32_t mip_levels = 1,
			uint32_t base_array_layer = 0,
			uint32_t layer_count = 1);

		// offscreen levels

		bool RequestOffscreenLevels(const std::vector<void*>& required_levels, VkFormat depth_format);
		const std::vector<OffscreenPassResources>& GetActiveOffscreenLevels() const { return _active_offscreen_levels; }
		VkImageView GetOffscreenLevelView(void* level);



		bool Version_1_3_OrHigher() const { return _api_version >= VK_API_VERSION_1_3; }
		bool Version_1_2_OrHigher() const { return _api_version >= VK_API_VERSION_1_2; }
		void FrameBufferResize(uint32_t width, uint32_t height);

	private:
		void _use_lastest_api_version();
		QueueFamilyIndices _find_queue_families(VkPhysicalDevice device);
		SwapChainSupportDetails _query_swap_chain_support(VkPhysicalDevice device);
		bool _check_device_extension_support(VkPhysicalDevice device);
		bool _check_and_enable_bindless_feature(VkPhysicalDevice device);
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
		VkFormat _find_supported_format(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
		bool _create_base_image_and_view(uint32_t width,
			uint32_t height,
			VkFormat format,
			VkImageUsageFlags usage,
			VkImageAspectFlags aspect_flags,
			OffscreenPassResources& out_resources);
		void _cleanup_offscreen_resource(OffscreenPassResources& res);
		void _cleanup_active_offscreen_levels();
		bool _cleanup_swap_chain();
		// now do nothing
		void _update_resizable_descriptor_sets();


#ifndef NDEBUG
		bool _check_validation_layers();
		bool _setup_debug_messenger();
#endif // !NDEBUG


	public:
		std::function<void(void)> WaitForWindowEvents = nullptr;

	private:
		VkInstance _instance = VK_NULL_HANDLE;
		VkDevice _device = VK_NULL_HANDLE;
		VkSurfaceKHR _surface = VK_NULL_HANDLE;
		VkPhysicalDevice _physical_device = VK_NULL_HANDLE;
		VkQueue _graphics_queue = VK_NULL_HANDLE;
		VkQueue _present_queue = VK_NULL_HANDLE;
		VkSwapchainKHR _swap_chain = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT _debug_messenger = VK_NULL_HANDLE;
		
		VkRenderPass _render_pass_ui = VK_NULL_HANDLE;
		std::array<OffscreenPassResources, MAX_FRAMES_IN_FLIGHT> _shadow_pass_res;
		std::array<OffscreenPassResources, MAX_FRAMES_IN_FLIGHT> _main_scene_res;
		std::array<OffscreenPassResources, MAX_FRAMES_IN_FLIGHT> _main_scene_depth_views;
		std::array<OffscreenPassResources, MAX_FRAMES_IN_FLIGHT> _postprocess_res;
		// 臨時緩存的配置參數（用於 Resize 重建）
		OffscreenPassResources _temp_offscreen_depth_view = {};
		// 容納 0 到 N 個離屏關卡資产
		std::vector<OffscreenPassResources> _active_offscreen_levels;

		VkPhysicalDeviceDescriptorIndexingFeaturesEXT _enabled_indexing_features = {};

		std::vector<VkImage> _swap_chain_images;
		std::vector<VkImageView> _swap_chain_image_views;
		std::vector<VkFramebuffer> _swap_chain_framebuffers;

		SwapChainSupportDetails _swap_chain_support;
		QueueFamilyIndices _queue_family_indices;
		VkExtent2D _swap_chain_extent = {};

		VkFormat _swap_chain_image_format = VK_FORMAT_UNDEFINED;
		VkImageUsageFlags _swap_chain_image_usages = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		VkFormat _depth_format = VK_FORMAT_UNDEFINED;
		uint32_t _api_version = VK_API_VERSION_1_0;
		uint32_t _frame_buffer_width = 0;
		uint32_t _frame_buffer_height = 0;
		uint32_t _swap_chain_image_count = 0;

		bool _framebuffer_resized = false;
		

	};

}

#endif // !_VULKANBASE_



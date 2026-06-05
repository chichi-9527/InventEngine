#include "IEpch.h"
#include "VulkanBase.h"

namespace INVENT
{

	static VmaAllocator vmaAllocator = nullptr;
	static std::unordered_map<VkBuffer, VmaAllocation> MapBufferAllocation;
	static std::unordered_map<VkImage, VmaAllocation> MapImageAllocation;

	constexpr uint32_t OffscreenLevelSizeX = 1024;
	constexpr uint32_t OffscreenLevelSizeY = 1024;
	constexpr uint32_t TempLevelDepthSizeX = 1024;
	constexpr uint32_t TempLevelDepthSizeY = 1024;
	constexpr uint32_t ShadowMapSizeX = 2048;
	constexpr uint32_t ShadowMapSizeY = 2048;

	// layers
	static std::vector<const char*> validationLayers;
	static std::vector<const char*> instanceExtensions;
	static std::vector<const char*> deviceExtensions;

#ifndef NDEBUG
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{

		switch (messageSeverity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			INVENT_LOG_TRACE(std::format("VERBOSE: {} \n", pCallbackData->pMessage));
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			INVENT_LOG_INFO(std::format("INFO: {} \n", pCallbackData->pMessage));
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			INVENT_LOG_WARNING(std::format("WARNING: {} \n", pCallbackData->pMessage));
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			INVENT_LOG_ERROR(std::format("ERROR: {} \n", pCallbackData->pMessage));
			break;
		default:
			break;
		}

		return VK_FALSE;
	}
#endif // !NDEBUG

	VulkanBase::~VulkanBase()
	{}

	void VulkanBase::AddValidationLayer(const char* layerName)
	{
		validationLayers.push_back(layerName);
	}

	void VulkanBase::AddInstanceExtension(const char* extensionName)
	{
		instanceExtensions.push_back(extensionName);
	}

	void VulkanBase::AddDeviceExtension(const char* extensionName)
	{
		deviceExtensions.push_back(extensionName);
	}

	VulkanBase& VulkanBase::Base()
	{
		static VulkanBase base;
		return base;
	}

	bool VulkanBase::CreateVulkanInstance()
	{
		_use_lastest_api_version();
		INVENT_LOG_INFO(std::format("[VulkanBase] vulkan api version : {}\n", _api_version));

#ifndef NDEBUG
		AddValidationLayer("VK_LAYER_KHRONOS_validation");
		AddInstanceExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif // !NDEBUG


		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Vulkan App";
		appInfo.apiVersion = _api_version;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

#ifndef NDEBUG
		if (auto check = !_check_validation_layers())
		{
			return false;
		}
		createInfo.enabledLayerCount = uint32_t(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
		debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugCreateInfo.pfnUserCallback = debugCallback;

		createInfo.pNext = (void*)&debugCreateInfo;
#else
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
#endif // !NDEBUG
		createInfo.enabledExtensionCount = uint32_t(instanceExtensions.size());
		createInfo.ppEnabledExtensionNames = instanceExtensions.data();

		if (VkResult result = vkCreateInstance(&createInfo, nullptr, &_instance))
		{
			INVENT_LOG_ERROR("[VulkanBase] failed to create instance!\n");
			return false;
		}

		INVENT_LOG_INFO(std::format("INFO : Create Instance done, instance Extensions Num : {}  :\n", instanceExtensions.size()));
		for (auto& name : instanceExtensions)
		{
			INVENT_LOG_INFO(std::format("\t {} ;\n", name));
		}

#ifndef NDEBUG
		return _setup_debug_messenger();
#endif // !NDEBUG

		return true;
	}

	void VulkanBase::CleanUp()
	{
		if (_device != VK_NULL_HANDLE)
		{
			vkDeviceWaitIdle(_device);
		}

		_cleanup_swap_chain();

		// 2. 清理所有離屏 Passes 的 Image、ImageView 和 Framebuffer 資源
		// 清理陰影圖 Pass 資源
		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			_cleanup_offscreen_resource(_shadow_pass_res[i]);
		}

		// 清理主場景顏色與深度 Pass 資源
		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			_cleanup_offscreen_resource(_main_scene_res[i]);
			_cleanup_offscreen_resource(_main_scene_depth_views[i]);
		}

		// 清理後處理 Pass 資源
		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			_cleanup_offscreen_resource(_postprocess_res[i]);
		}

		// 清理最新修改的臨時離屏深度資源
		_cleanup_offscreen_resource(_temp_offscreen_depth_view);

		// 清理動態加載的關卡離屏資源陣列
		for (auto& levelRes : _active_offscreen_levels)
		{
			_cleanup_offscreen_resource(levelRes);
		}
		_active_offscreen_levels.clear();

		// 3. 銷毀所有的 RenderPass 控制代碼
		// 銷毀 UI RenderPass
		if (_render_pass_ui != VK_NULL_HANDLE)
		{
			vkDestroyRenderPass(_device, _render_pass_ui, nullptr);
			_render_pass_ui = VK_NULL_HANDLE;
		}

		// 銷毀各個離屏資源結構體內存儲的 RenderPass（若在初始化時為每個 Frame 分配了獨立的 RenderPass）
		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			if (_shadow_pass_res[i].RenderPass != VK_NULL_HANDLE)
			{
				vkDestroyRenderPass(_device, _shadow_pass_res[i].RenderPass, nullptr);
				_shadow_pass_res[i].RenderPass = VK_NULL_HANDLE;
			}
			if (_main_scene_res[i].RenderPass != VK_NULL_HANDLE)
			{
				vkDestroyRenderPass(_device, _main_scene_res[i].RenderPass, nullptr);
				_main_scene_res[i].RenderPass = VK_NULL_HANDLE;
			}
			if (_postprocess_res[i].RenderPass != VK_NULL_HANDLE)
			{
				vkDestroyRenderPass(_device, _postprocess_res[i].RenderPass, nullptr);
				_postprocess_res[i].RenderPass = VK_NULL_HANDLE;
			}
		}

		//  銷毀 VMA Allocator
		DestroyVmaAllocator();

		// 5. 銷毀邏輯設備
		if (_device != VK_NULL_HANDLE)
		{
			vkDestroyDevice(_device, nullptr);
			_device = VK_NULL_HANDLE;
		}

		// 6. 銷毀視窗表面 (Surface)
		if (_surface != VK_NULL_HANDLE)
		{
			vkDestroySurfaceKHR(_instance, _surface, nullptr);
			_surface = VK_NULL_HANDLE;
		}

		// 7. 銷毀偵錯回呼信使 (Debug Messenger)
		if (_debug_messenger != VK_NULL_HANDLE)
		{
			// 由於 vkDestroyDebugUtilsMessengerEXT 是擴展函數，需要透過動態代理或 PFN 呼叫
			auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(_instance, "vkDestroyDebugUtilsMessengerEXT");
			if (func != nullptr)
			{
				func(_instance, _debug_messenger, nullptr);
			}
			_debug_messenger = VK_NULL_HANDLE;
		}

		// 8. 最後銷毀 Vulkan 實例 (Instance)
		if (_instance != VK_NULL_HANDLE)
		{
			vkDestroyInstance(_instance, nullptr);
			_instance = VK_NULL_HANDLE;
		}

	}

	bool VulkanBase::PickPhysicalDevice()
	{
		uint32_t deviceCount = 0;
		if (VkResult result = vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr))
		{
			INVENT_LOG_ERROR(std::format(" [ VulkanBase ] Failed to get the count of physical devices! Error code: {}\n", int32_t(result)));
			return false;
		}
		if (deviceCount == 0)
		{
			INVENT_LOG_ERROR(std::format(" [ VulkanBase ] Failed to find GPUs with Vulkan support!\n"));
			return false;
		}
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data());

		// 选择显卡
		std::string deviceNmae;
		auto isDeviceSuitable = [this, &deviceNmae](VkPhysicalDevice device)->bool {
			VkPhysicalDeviceProperties deviceProperties;
			VkPhysicalDeviceFeatures deviceFeatures;
			vkGetPhysicalDeviceProperties(device, &deviceProperties);
			vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

			_queue_family_indices = _find_queue_families(device);

			auto swapChainAdequate = [this, device]()->bool {
				_swap_chain_support = _query_swap_chain_support(device);
				return !_swap_chain_support.Formats.empty() && !_swap_chain_support.PresentModes.empty();
				};

			deviceNmae = deviceProperties.deviceName;

			return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
				&& deviceFeatures.geometryShader
				&& deviceFeatures.samplerAnisotropy
				&& _queue_family_indices.IsComplete()
				&& _check_device_extension_support(device)
				&& swapChainAdequate();
			};

		for (const auto& device : devices)
		{
			if (isDeviceSuitable(device))
			{
				_physical_device = device;
				break;
			}
		}

		if (_physical_device == VK_NULL_HANDLE)
		{
			INVENT_LOG_ERROR(" [ VulkanBase ] failed to find a suitable GPU!\n");
			return false;
		}

		INVENT_LOG_INFO(std::format("[ VulkanBase ] device name : {} \n", deviceNmae));

		return true;
	}

	bool VulkanBase::CreateLogicalDevice()
	{
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { _queue_family_indices.GraphicsFamily,_queue_family_indices.PresentFamily };
		float queuePriority = 1.0f;
		for (auto queueFamily : uniqueQueueFamilies)
		{
			auto& queueCreateInfo = queueCreateInfos.emplace_back();
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
		}

		VkPhysicalDeviceFeatures deviceFeatures{};
		deviceFeatures.samplerAnisotropy = VK_TRUE;

		VkPhysicalDeviceVulkan11Features feat11 = {};
		feat11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
		feat11.shaderDrawParameters = VK_TRUE;

		VkDeviceCreateInfo deviceCreateInfo{};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
		deviceCreateInfo.queueCreateInfoCount = (uint32_t)queueCreateInfos.size();
		deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
		deviceCreateInfo.enabledExtensionCount = (uint32_t)deviceExtensions.size();
		deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
#ifndef NDEBUG
		deviceCreateInfo.enabledLayerCount = (uint32_t)validationLayers.size();
		deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
#else
		deviceCreateInfo.enabledLayerCount = 0;
#endif
		deviceCreateInfo.pNext = &feat11;


		if (VkResult result = vkCreateDevice(_physical_device, &deviceCreateInfo, nullptr, &_device))
		{
			INVENT_LOG_ERROR(std::format(" [ VulkanBase ] failed to create logical device! Error code: {}\n", int32_t(result)));
			return false;
		}

		vkGetDeviceQueue(_device, _queue_family_indices.GraphicsFamily, 0, &_graphics_queue);
		vkGetDeviceQueue(_device, _queue_family_indices.PresentFamily, 0, &_present_queue);

		return true;
	}

	bool VulkanBase::CreateSwapChain()
	{
		auto surfaceFormat = _choose_swap_surface_format(_swap_chain_support.Formats);
		auto presentMode = _choose_swap_presenta_mode(_swap_chain_support.PresentModes);

		// 获取最新的 Surface Capabilities
		if (VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_physical_device, _surface, &_swap_chain_support.Capabilities))
		{
			std::cout << std::format("ERROR : [ VulkanBase ] Failed to Get Physical Device Surface Capabilities! Error code: {}\n", int32_t(result));
			return false;
		}
		auto extent = _choose_swap_extent(_swap_chain_support.Capabilities);

		uint32_t imageCount = _swap_chain_support.Capabilities.minImageCount + 1;
		if (_swap_chain_support.Capabilities.maxImageCount > 0 && imageCount > _swap_chain_support.Capabilities.maxImageCount)
		{
			imageCount = _swap_chain_support.Capabilities.maxImageCount;
		}
		// 判断支持功能
		VkSurfaceCapabilitiesKHR surfaceCapabilities;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_physical_device, _surface, &surfaceCapabilities);
		// For screenshot
		if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
		{
			_swap_chain_image_usages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		}
		// For previous frame sampling
		if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_SAMPLED_BIT)
		{
			_swap_chain_image_usages |= VK_IMAGE_USAGE_SAMPLED_BIT;
		}

		// VkSwapchainCreateInfoKHR
		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = _surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = _swap_chain_image_usages;

		uint32_t queueFamilyIndices[] = { _queue_family_indices.GraphicsFamily, _queue_family_indices.PresentFamily };
		if (_queue_family_indices.GraphicsFamily != _queue_family_indices.PresentFamily)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}
		createInfo.preTransform = _swap_chain_support.Capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		//
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		// 
		if (VkResult result = vkCreateSwapchainKHR(_device, &createInfo, nullptr, &_swap_chain))
		{
			INVENT_LOG_ERROR(std::format(" [ VulkanBase ] Failed to create a swapchain! Error code: {}\n", int32_t(result)));
			return false;
		}

		uint32_t swapchainImageCount;
		if (VkResult result = vkGetSwapchainImagesKHR(_device, _swap_chain, &swapchainImageCount, nullptr))
		{
			INVENT_LOG_ERROR(std::format(" [ VulkanBase ] Failed to get the count of swapchain images! Error code: {}\n", int32_t(result)));
			return false;
		}
		_swap_chain_images.resize(swapchainImageCount);
		if (VkResult result = vkGetSwapchainImagesKHR(_device, _swap_chain, &swapchainImageCount, _swap_chain_images.data()))
		{
			INVENT_LOG_ERROR(std::format(" [ VulkanBase ] Failed to get swapchain images! Error code: {}\n", int32_t(result)));
			return false;
		}
		_swap_chain_image_format = surfaceFormat.format;
		_swap_chain_extent = extent;
		_swap_chain_image_count = swapchainImageCount;

		return true;
	}

	bool VulkanBase::CreateSwapChainImageView()
	{
		_swap_chain_image_views.resize(_swap_chain_images.size());
		for (size_t i = 0; i < _swap_chain_images.size(); ++i)
		{
			_swap_chain_image_views[i] = _create_image_view(_swap_chain_images[i], _swap_chain_image_format, VK_IMAGE_ASPECT_COLOR_BIT);
			if (_swap_chain_image_views[i] == nullptr)
			{
				INVENT_LOG_ERROR(std::format(" [ VulkanBase ] Failed to Create SwapChainImageView! the number: {}\n", i));
				return false;
			}
		}

		return true;
	}

	bool VulkanBase::CreateVmaAllocator()
	{
		VmaVulkanFunctions vulkanFunctions = {};
		vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
		vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

		VmaAllocatorCreateInfo allocatorCreateInfo = {};
		allocatorCreateInfo.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
		allocatorCreateInfo.vulkanApiVersion = _api_version;
		allocatorCreateInfo.physicalDevice = _physical_device;
		allocatorCreateInfo.device = _device;
		allocatorCreateInfo.instance = _instance;
		allocatorCreateInfo.pVulkanFunctions = &vulkanFunctions;

		if (VkResult result = vmaCreateAllocator(&allocatorCreateInfo, &vmaAllocator))
		{
			INVENT_LOG_ERROR(" [VulkanBase] Create Vma Allocator error! \n");
			return false;
		}
		INVENT_LOG_INFO(" [VulkanBase] Create Vma Allocator done. \n");
		return true;
	}

	void VulkanBase::DestroyVmaAllocator()
	{
		if (vmaAllocator != nullptr)
		{
			vmaDestroyAllocator(vmaAllocator);
			vmaAllocator = nullptr;
		}
	}

	bool VulkanBase::GetDepthFormat()
	{
		_depth_format = _find_supported_format(
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);
		if (VK_FORMAT_UNDEFINED == _depth_format)
		{
			return false;
		}
		return true;
	}

	bool VulkanBase::CreateDefaultRenderPasses()
	{
		// shadowMap
		{
			VkFormat shadowDepthFormat = _find_supported_format(
				{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D16_UNORM },
				VK_IMAGE_TILING_OPTIMAL,
				VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT
			);
			if (VK_FORMAT_UNDEFINED == shadowDepthFormat)
			{
				return false;
			}
			VkAttachmentDescription depth = {};
			depth.format = shadowDepthFormat;
			depth.samples = VK_SAMPLE_COUNT_1_BIT;
			depth.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			depth.storeOp = VK_ATTACHMENT_STORE_OP_STORE;          // 保留阴影图
			depth.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			depth.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depth.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			depth.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			std::vector<VkAttachmentDescription> attachments = { depth };

			Subpass shaderMapSub0;
			shaderMapSub0.DepthStencilRef = { 0,VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };

			std::vector<Subpass> subpasses = { shaderMapSub0 };

			VkSubpassDependency dependency{};
			dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			dependency.dstSubpass = 0;
			dependency.srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			dependency.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			dependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			dependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			std::vector<VkSubpassDependency> dependencies = { dependency };

			for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
			{
				_shadow_pass_res[i].RenderPass = CreateRenderPass(attachments, subpasses, dependencies);
				if (VK_NULL_HANDLE == _shadow_pass_res[i].RenderPass)
				{
					return false;
				}
			}

		}

		// main
		{
			VkAttachmentDescription mainColorAttachment{};
			mainColorAttachment.format = _swap_chain_image_format;
			mainColorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
			mainColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			mainColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			mainColorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			mainColorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			VkAttachmentDescription mainDepthAttachment{};
			mainDepthAttachment.format = _depth_format;
			mainDepthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
			mainDepthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			mainDepthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			mainDepthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			mainDepthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			std::vector<VkAttachmentDescription> attachments = { mainColorAttachment, mainDepthAttachment };

			Subpass mainSub0;
			mainSub0.ColorRefs = { {0,VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL} };
			mainSub0.DepthStencilRef = { 1,VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };

			std::vector<Subpass> subpasses = { mainSub0 };

			VkSubpassDependency dependency{};
			dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			dependency.dstSubpass = 0;
			dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
			dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			dependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			dependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			std::vector<VkSubpassDependency> dependencies = { dependency };

			for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
			{
				_main_scene_res[i].RenderPass = CreateRenderPass(attachments, subpasses, dependencies);
				if (VK_NULL_HANDLE == _main_scene_res[i].RenderPass)
				{
					return false;
				}
			}

		}

		// postprocess
		{
			VkAttachmentDescription color = {};
			color.format = _swap_chain_image_format;
			color.samples = VK_SAMPLE_COUNT_1_BIT;
			color.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
			color.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			color.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			color.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			color.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			color.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			std::vector<VkAttachmentDescription> attachments = { color };

			Subpass shaderMapSub0;
			shaderMapSub0.ColorRefs = { { 0,VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } };

			std::vector<Subpass> subpasses = { shaderMapSub0 };

			VkSubpassDependency dependency{};
			dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			dependency.dstSubpass = 0;
			dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			std::vector<VkSubpassDependency> dependencies = { dependency };

			for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
			{
				_postprocess_res[i].RenderPass = CreateRenderPass(attachments, subpasses, dependencies);
				if (VK_NULL_HANDLE == _postprocess_res[i].RenderPass)
				{
					return false;
				}
			}
		}

		// ui
		{
			VkAttachmentDescription color = {};
			color.format = _swap_chain_image_format;
			color.samples = VK_SAMPLE_COUNT_1_BIT;
			color.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
			color.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			color.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			color.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

			std::vector<VkAttachmentDescription> attachments = { color };

			Subpass uiSub0;
			uiSub0.ColorRefs = { { 0,VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } };

			std::vector<Subpass> subpasses = { uiSub0 };

			VkSubpassDependency dependency{};
			dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			dependency.dstSubpass = 0;
			dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			std::vector<VkSubpassDependency> dependencies = { dependency };

			_render_pass_ui = CreateRenderPass(attachments, subpasses, dependencies);

			if (VK_NULL_HANDLE == _render_pass_ui)
			{
				return false;
			}
		}

		return true;
	}

	bool VulkanBase::CreateDefaultFramebuffers()
	{
		uint32_t w = _swap_chain_extent.width;
		uint32_t h = _swap_chain_extent.height;

		_swap_chain_framebuffers.resize(_swap_chain_image_views.size());
		for (size_t i = 0; i < _swap_chain_image_views.size(); ++i)
		{
			// 与最后一个 renderpass 结构一样
			VkImageView attachments[] = { _swap_chain_image_views[i] };

			_swap_chain_framebuffers[i] = CreateFramebuffer(w,
				h,
				_render_pass_ui,
				1,
				attachments);

			if (VK_NULL_HANDLE == _swap_chain_framebuffers[i])
			{
				return false;
			}
		}

		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			// shadow map
			{
				VkImageView attachments[] = {
					_shadow_pass_res[i].DepthView
				};
				_shadow_pass_res[i].Framebuffer = CreateFramebuffer(ShadowMapSizeX,
					ShadowMapSizeY,
					_shadow_pass_res[i].RenderPass,
					1,
					attachments);
				if (VK_NULL_HANDLE == _shadow_pass_res[i].Framebuffer)
				{
					return false;
				}
			}

			// main & postprocess
			{
				VkImageView mainAttachments[] = {
					_main_scene_res[i].View,
					_main_scene_depth_views[i].View
				};
				_main_scene_res[i].Framebuffer = CreateFramebuffer(w,
					h,
					_main_scene_res[i].RenderPass,
					2,
					mainAttachments);
				if (VK_NULL_HANDLE == _main_scene_res[i].Framebuffer)
				{
					return false;
				}

				VkImageView postprocessAttachments[] = {
					_postprocess_res[i].View,
				};
				_postprocess_res[i].Framebuffer = CreateFramebuffer(w,
					h,
					_postprocess_res[i].RenderPass,
					1,
					postprocessAttachments);
				if (VK_NULL_HANDLE == _postprocess_res[i].Framebuffer)
				{
					return false;
				}
			}
		}

		return true;
	}

	bool VulkanBase::InitializeAllOffscreenPasses()
	{
		// shadow map
		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			if (!_create_base_image_and_view(ShadowMapSizeX,
				ShadowMapSizeY,
				_depth_format,
				VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				VK_IMAGE_ASPECT_DEPTH_BIT,
				_shadow_pass_res[i]))
			{
				INVENT_LOG_ERROR(std::format(" [ VulkanBase ] [ InitializeAllOffscreenPasses ] [shadow map] Failed to create image and view. \n"));
				return false;
			}
			_shadow_pass_res[i].DynamicFormat = _depth_format;
		}

		// temp level depth
		if (!_create_base_image_and_view(TempLevelDepthSizeX,
			TempLevelDepthSizeY,
			_depth_format,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			VK_IMAGE_ASPECT_DEPTH_BIT,
			_temp_offscreen_depth_view))
		{
			INVENT_LOG_ERROR(std::format(" [ VulkanBase ] [ InitializeAllOffscreenPasses ] [temp level depth] Failed to create image and view. \n"));
			return false;
		}

		if (!RecreateResizableResources())
		{
			return false;
		}

		return true;
	}

	bool VulkanBase::RecreateResizableResources()
	{
		uint32_t w = _swap_chain_extent.width;
		uint32_t h = _swap_chain_extent.height;

		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			_cleanup_offscreen_resource(_main_scene_res[i]);
			_cleanup_offscreen_resource(_main_scene_depth_views[i]);
			_cleanup_offscreen_resource(_postprocess_res[i]);

			// main color
			if (!_create_base_image_and_view(w,
				h,
				_swap_chain_image_format,
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				VK_IMAGE_ASPECT_COLOR_BIT,
				_main_scene_res[i]))
			{
				INVENT_LOG_ERROR(std::format(" [ VulkanBase ] [ RecreateResizeResources ] [main color] Failed to create image and view. \n"));
				return false;
			}
			_main_scene_res[i].DynamicFormat = _swap_chain_image_format;

			// main depth
			if (!_create_base_image_and_view(w,
				h,
				_depth_format,
				VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
				VK_IMAGE_ASPECT_DEPTH_BIT,
				_main_scene_depth_views[i]))
			{
				INVENT_LOG_ERROR(std::format(" [ VulkanBase ] [ RecreateResizeResources ] [main depth] Failed to create image and view. \n"));
				return false;
			}

			// postprocess
			if (!_create_base_image_and_view(w,
				h,
				_swap_chain_image_format,
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				VK_IMAGE_ASPECT_COLOR_BIT,
				_postprocess_res[i]))
			{
				INVENT_LOG_ERROR(std::format(" [ VulkanBase ] [ RecreateResizeResources ] [postprocess] Failed to create image and view. \n"));
				return false;
			}
			_postprocess_res[i].DynamicFormat = _swap_chain_image_format;

		}

		return true;
	}

	bool VulkanBase::RecreateSwapChain()
	{
		while (_frame_buffer_width == 0 || _frame_buffer_height == 0)
		{
			if (WaitForWindowEvents)
			{
				WaitForWindowEvents();
			}
		}

		vkDeviceWaitIdle(_device);

		if (!_cleanup_swap_chain() ||
			!CreateSwapChain() ||
			!CreateSwapChainImageView())
		{
			return false;
		}
		if (!RecreateResizableResources())
		{
			INVENT_LOG_ERROR(" [ VulkanBase ] Failed to recreate resizable offscreen resources.\n");
			return false;
		}
		if (Version_1_3_OrHigher() && !CreateDefaultFramebuffers())
		{
			INVENT_LOG_ERROR(" [ VulkanBase ] Failed to recreate default framebuffers.\n");
			return false;
		}
		
		_update_resizable_descriptor_sets();

		return true;
	}

	VkRenderPass VulkanBase::CreateRenderPass(std::vector<VkAttachmentDescription>& attachments, 
		std::vector<Subpass>& subpasses, 
		std::vector<VkSubpassDependency>& dependencies)
	{
		std::vector<VkSubpassDescription> subpassDesc(subpasses.size());
		for (size_t i = 0; i < subpasses.size(); ++i)
		{
			const auto& in = subpasses[i];
			auto& out = subpassDesc[i];
			out.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			out.inputAttachmentCount = (uint32_t)in.InputRefs.size();
			out.pInputAttachments = in.InputRefs.data();
			out.colorAttachmentCount = (uint32_t)in.ColorRefs.size();
			out.pColorAttachments = in.ColorRefs.data();
			out.pDepthStencilAttachment = (in.DepthStencilRef.attachment != VK_ATTACHMENT_UNUSED) ? &in.DepthStencilRef : nullptr;
			out.preserveAttachmentCount = (uint32_t)in.PreserveAttachment.size();
			out.pPreserveAttachments = in.PreserveAttachment.data();
		}

		VkRenderPassCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		createInfo.attachmentCount = (uint32_t)attachments.size();
		createInfo.pAttachments = attachments.data();
		createInfo.subpassCount = (uint32_t)subpassDesc.size();
		createInfo.pSubpasses = subpassDesc.data();
		createInfo.dependencyCount = (uint32_t)dependencies.size();
		createInfo.pDependencies = dependencies.data();

		VkRenderPass renderPass;
		if (VkResult result = vkCreateRenderPass(_device, &createInfo, nullptr, &renderPass))
		{
			INVENT_LOG_ERROR(std::format(" [ VulkanBase ] Failed to create render pass! Error code: {}\n", int32_t(result)));
			return VK_NULL_HANDLE;
		}

		return renderPass;

	}

	VkFramebuffer VulkanBase::CreateFramebuffer(uint32_t width,
		uint32_t height,
		VkRenderPass render_pass,
		uint32_t attachment_size,
		VkImageView* attachments,
		uint32_t layers)
	{
		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = render_pass;
		framebufferInfo.attachmentCount = attachment_size;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = width;
		framebufferInfo.height = height;
		framebufferInfo.layers = layers;

		VkFramebuffer frameBuffer = VK_NULL_HANDLE;
		if (VkResult result = vkCreateFramebuffer(_device, &framebufferInfo, nullptr, &frameBuffer))
		{
			INVENT_LOG_ERROR(std::format(" [ VulkanBase ] Failed to create framebuffer! Error code: {}\n", int32_t(result)));
			return VK_NULL_HANDLE;
		}

		return frameBuffer;
	}

	bool VulkanBase::UseVmaCreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer)
	{
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo vmaAllocInfo{};
		vmaAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;
		if (properties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
		{
			vmaAllocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
		}

		VmaAllocation allocation;
		if (VkResult result = vmaCreateBuffer(vmaAllocator, &bufferInfo, &vmaAllocInfo, &buffer, &allocation, nullptr))
		{
			INVENT_LOG_ERROR(std::format(" [ VulkanBase ] Failed to create buffer! VkBufferUsageFlags : {},  Error code: {}\n", int32_t(usage), int32_t(result)));
			return false;
		}

		MapBufferAllocation[buffer] = allocation;

		return true;
	}

	void VulkanBase::UseVmaDestroyBuffer(VkBuffer buffer)
	{
		auto iter = MapBufferAllocation.find(buffer);
		if (iter != MapBufferAllocation.end())
			vmaDestroyBuffer(vmaAllocator, buffer, iter->second);
		MapBufferAllocation.erase(buffer);
	}

	bool VulkanBase::UseVmaCreateImage(uint32_t width, 
		uint32_t height, 
		uint32_t mip_levels,
		VkFormat format, 
		VkImageTiling tiling, 
		VkImageUsageFlags usage, 
		VkMemoryPropertyFlags properties, 
		VkImage & image)
	{
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = mip_levels;
		imageInfo.arrayLayers = 1;
		//
		imageInfo.format = format;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usage;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.flags = 0; // Optional

		VmaAllocationCreateInfo vmaAllocInfo{};
		vmaAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;
		if (properties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
		{
			vmaAllocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
		}

		VmaAllocation allocation;
		if (VkResult result = vmaCreateImage(vmaAllocator, &imageInfo, &vmaAllocInfo, &image, &allocation, nullptr))
		{
			INVENT_LOG_ERROR(std::format(" [ VulkanBase ] Failed to create image!  Error code: {}\n", int32_t(result)));
			return false;
		}

		MapImageAllocation[image] = allocation;

		return true;
	}

	void VulkanBase::UseVmaDestroyImage(VkImage image)
	{
		auto iter = MapImageAllocation.find(image);
		if (iter != MapImageAllocation.end())
			vmaDestroyImage(vmaAllocator, image, iter->second);
		MapImageAllocation.erase(image);
	}

	VkImageView VulkanBase::CreateImageView(VkImage image,
		VkFormat format,
		VkImageAspectFlags aspect_flags,
		VkImageViewType view_type,
		uint32_t mip_levels,
		uint32_t base_array_layer,
		uint32_t layer_count)
	{
		return _create_image_view(image, format, aspect_flags, view_type, mip_levels, base_array_layer, layer_count);
	}

	bool VulkanBase::RequestOffscreenLevels(const std::vector<void*>& required_levels, VkFormat depth_format)
	{
		_cleanup_active_offscreen_levels();

		if (required_levels.empty())
			return true;

		_active_offscreen_levels.reserve(required_levels.size());
		for (void* level : required_levels)
		{
			OffscreenPassResources newLevel;
			newLevel.Level = level;
			newLevel.DynamicFormat = VK_FORMAT_B8G8R8A8_UNORM;

			// 創建顏色貼圖資產 
			if (!_create_base_image_and_view(OffscreenLevelSizeX,
				OffscreenLevelSizeY,
				VK_FORMAT_B8G8R8A8_UNORM,
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				VK_IMAGE_ASPECT_COLOR_BIT,
				newLevel))
			{
				INVENT_LOG_ERROR(std::format(" [ VulkanBase ] [ func RequestOffscreenLevels ] [ color ] Failed to create iamge and view. Level : {} \n", reinterpret_cast<uint64_t>(level)));
				return false;
			}

			// 創建深度貼圖資產 (專用於該關卡內部的消隱測試)
			OffscreenPassResources tempDepth;
			if (!_create_base_image_and_view(OffscreenLevelSizeX,
				OffscreenLevelSizeY,
				_depth_format,
				VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
				VK_IMAGE_ASPECT_DEPTH_BIT,
				tempDepth))
			{
				INVENT_LOG_ERROR(std::format(" [ VulkanBase ] [ func RequestOffscreenLevels ] [ depth ] Failed to create iamge and view. Level : {} \n", reinterpret_cast<uint64_t>(level)));
				return false;
			}

			newLevel.DepthView = tempDepth.View;

			if (!Version_1_3_OrHigher())
			{
				VkAttachmentDescription color{};
				color.format = VK_FORMAT_B8G8R8A8_UNORM;
				color.samples = VK_SAMPLE_COUNT_1_BIT;
				color.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				color.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
				color.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				color.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

				VkAttachmentDescription depth = {};
				depth.format = _depth_format;
				depth.samples = VK_SAMPLE_COUNT_1_BIT;
				depth.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				depth.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				depth.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				depth.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

				std::vector<VkAttachmentDescription> attachments = { color, depth };

				Subpass offscreenSub0;
				offscreenSub0.ColorRefs = { {0,VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL} };
				offscreenSub0.DepthStencilRef = { 1,VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };

				std::vector<Subpass> subpasses = { offscreenSub0 };
				std::vector<VkSubpassDependency> dependencies = {};

				newLevel.RenderPass = CreateRenderPass(attachments, subpasses, dependencies);

				if (VK_NULL_HANDLE == newLevel.RenderPass)
				{
					INVENT_LOG_ERROR(std::format(" [ VulkanBase ] [ func RequestOffscreenLevels ] Failed to create render pass. Level : {} \n", reinterpret_cast<uint64_t>(level)));
					return false;
				}

				VkImageView viewAttachments[] = { newLevel.View, newLevel.DepthView };

				newLevel.Framebuffer = CreateFramebuffer(OffscreenLevelSizeX,
					OffscreenLevelSizeY,
					newLevel.RenderPass,
					2,
					viewAttachments);

			}
			
			_active_offscreen_levels.push_back(newLevel);

		}

		return true;
	}

	VkImageView VulkanBase::GetOffscreenLevelView(void* level)
	{
		for (const auto& res : _active_offscreen_levels)
		{
			if (res.Level == level)
			{
				return res.View;
			}
		}
		return VK_NULL_HANDLE;
	}

	void VulkanBase::FrameBufferResize(uint32_t width, uint32_t height)
	{
		_frame_buffer_width = width;
		_frame_buffer_height = height;
		_framebuffer_resized = true;
	}

	void VulkanBase::_use_lastest_api_version()
	{
		if (vkGetInstanceProcAddr(VK_NULL_HANDLE, "vkEnumerateInstanceVersion"))
		{
			if (VkResult result = vkEnumerateInstanceVersion(&_api_version))
			{
				INVENT_LOG_ERROR(std::format("[VulkanBase] vkEnumerateInstanceVersion error : {}  \n", (int)result));
			}
			return;
		}
		INVENT_LOG_ERROR(std::format("[VulkanBase] not found vkEnumerateInstanceVersion  \n"));
	}

	VulkanBase::QueueFamilyIndices VulkanBase::_find_queue_families(VkPhysicalDevice device)
	{
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		uint32_t i = 0;
		for (const auto& queueFamily : queueFamilies)
		{
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.GraphicsFamily = i;
				indices.HasGraphicsFamily = true;
			}

			VkBool32 presentSupport = false;
			if (VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(device, i, _surface, &presentSupport))
			{
				INVENT_LOG_ERROR(std::format("  [ VulkanBase ] failed to get device surface! Error code: {}\n", int32_t(result)));
			}

			if (presentSupport)
			{
				indices.PresentFamily = i;
				indices.HasPresentFamily = true;
			}

			if (indices.IsComplete()) break;

			i++;
		}

		return indices;
	}

	VulkanBase::SwapChainSupportDetails VulkanBase::_query_swap_chain_support(VkPhysicalDevice device)
	{
		SwapChainSupportDetails details;
		if (VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, _surface, &details.Capabilities))
		{
			INVENT_LOG_ERROR(std::format(" [ VulkanBase ] Failed to get physical device surface capabilities! Error code: {}\n", int32_t(result)));
			return details;
		}

		uint32_t surfaceFormatCount;
		if (VkResult result = vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &surfaceFormatCount, nullptr))
		{
			INVENT_LOG_ERROR(std::format(" [ VulkanBase ] Failed to get the count of surface formats! Error code: {}\n", int32_t(result)));
			return details;
		}
		if (surfaceFormatCount)
		{
			details.Formats.resize((size_t)surfaceFormatCount);
			if (VkResult result = vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &surfaceFormatCount, details.Formats.data()))
			{
				INVENT_LOG_ERROR(std::format(" [ VulkanBase ] Failed to get surface formats! Error code: {}\n", int32_t(result)));
				details.Formats.clear();
				return details;
			}
		}
		else
		{
			INVENT_LOG_ERROR(std::format(" [ VulkanBase ] Failed to find any supported surface format!\n"));
		}

		uint32_t surfacePresentModeCount;
		if (VkResult result = vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &surfacePresentModeCount, nullptr))
		{
			INVENT_LOG_ERROR(std::format(" [ VulkanBase ] Failed to get the count of surface present modes! Error code: {}\n", int32_t(result)));
			return details;
		}
		if (surfacePresentModeCount)
		{
			details.PresentModes.resize(surfacePresentModeCount);
			if (VkResult result = vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &surfacePresentModeCount, details.PresentModes.data()))
			{
				INVENT_LOG_ERROR(std::format(" [ VulkanBase ] Failed to get surface present modes!\nError code: {}\n", int32_t(result)));
				details.PresentModes.clear();
				return details;
			}
		}
		else
		{
			INVENT_LOG_ERROR(std::format(" [ VulkanBase ] Failed to find any surface present mode!\n"));
		}

		return details;
	}

	bool VulkanBase::_check_device_extension_support(VkPhysicalDevice device)
	{
		uint32_t extensionCount;
		if (VkResult result = vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr))
		{
			INVENT_LOG_ERROR(std::format(" [ VulkanBase ] Failed to get the count of device extension! Error code: {}\n", int32_t(result)));
			return false;
		}
		if (extensionCount)
		{
			std::vector<VkExtensionProperties> availableExtensions(extensionCount);
			if (VkResult result = vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data()))
			{
				INVENT_LOG_ERROR(std::format(" [ VulkanBase ] Failed to enumerate device extension properties! Error code: {}\n", int32_t(result)));
				return false;
			}
			bool found = false;
			for (auto& i : deviceExtensions)
			{
				for (auto& j : availableExtensions)
					if (!strcmp(i, j.extensionName))
					{
						found = true;
						break;
					}
				if (!found)
					i = nullptr;
			}
		}
		else
		{
			deviceExtensions.clear();
		}
		INVENT_LOG_INFO(std::format(" Check deviceExtensions done, Num : {}  :\n", deviceExtensions.size()));
		for (auto& name : deviceExtensions)
		{
			INVENT_LOG_INFO(std::format("\t {} \n", name));
		}
		return true;
	}

	VkSurfaceFormatKHR VulkanBase::_choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& available_formats)
	{
		for (const auto& availableFormat : available_formats)
		{
			if ((availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB) && (availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR))
			{
				return availableFormat;
			}
		}
		return available_formats[0];
	}

	VkPresentModeKHR VulkanBase::_choose_swap_presenta_mode(const std::vector<VkPresentModeKHR>& available_present_modes, VkPresentModeKHR mode)
	{
		for (const auto& availablePresentMode : available_present_modes)
		{
			if (availablePresentMode == mode)
			{
				return availablePresentMode;
			}
		}
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D VulkanBase::_choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			return capabilities.currentExtent;
		}
		else
		{
			VkExtent2D actualExtent = { _frame_buffer_width, _frame_buffer_height };
			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
			return actualExtent;
		}
	}

	VkImageView VulkanBase::_create_image_view(VkImage image, 
		VkFormat format, 
		VkImageAspectFlags aspect_flags,
		VkImageViewType view_type, 
		uint32_t mip_level,
		uint32_t base_array_layer,
		uint32_t layer_count)
	{
		VkImageViewCreateInfo imageViewCreateInfo{};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.image = image;
		imageViewCreateInfo.viewType = view_type;
		imageViewCreateInfo.format = format;
		imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.subresourceRange.aspectMask = aspect_flags;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = mip_level;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = base_array_layer;
		imageViewCreateInfo.subresourceRange.layerCount = layer_count;

		VkImageView imageView = nullptr;
		if (VkResult result = vkCreateImageView(_device, &imageViewCreateInfo, nullptr, &imageView))
		{
			return nullptr;
		}

		return imageView;
	}

	VkFormat VulkanBase::_find_supported_format(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
	{
		for (VkFormat format : candidates)
		{
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(_physical_device, format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
			{
				return format;
			}
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
			{
				return format;
			}
		}

		INVENT_LOG_ERROR(std::format(" [ VulkanBase ] Failed to find supported format!\n"));
		return VK_FORMAT_UNDEFINED;
	}

	bool VulkanBase::_create_base_image_and_view(uint32_t width, 
		uint32_t height, 
		VkFormat format, 
		VkImageUsageFlags usage, 
		VkImageAspectFlags aspect_flags, 
		OffscreenPassResources& out_resources)
	{
		// 離屏渲染目標的最佳平鋪模式通常是 VK_IMAGE_TILING_OPTIMAL
		VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;

		// 離屏渲染目標的最佳顯存類型通常是 GPU 純本地顯存 (VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
		VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

		auto imageCreated = UseVmaCreateImage(width, height, 1, format, tiling, usage, properties, out_resources.Image);
		if (!imageCreated || out_resources.Image == VK_NULL_HANDLE)
		{
			return false;
		}

		out_resources.View = _create_image_view(out_resources.Image, format, aspect_flags);
		if (out_resources.View == VK_NULL_HANDLE)
		{
			UseVmaDestroyImage(out_resources.Image);
			out_resources.Image = VK_NULL_HANDLE;
			return false;
		}

		return true;

	}

	void VulkanBase::_cleanup_offscreen_resource(OffscreenPassResources& res)
	{
		if (res.Framebuffer != VK_NULL_HANDLE)
		{
			vkDestroyFramebuffer(_device, res.Framebuffer, nullptr);
			res.Framebuffer = VK_NULL_HANDLE;
		}
		if (res.RenderPass != VK_NULL_HANDLE)
		{
			vkDestroyRenderPass(_device, res.RenderPass, nullptr);
			res.RenderPass = VK_NULL_HANDLE;
		}
		if (res.DepthView != VK_NULL_HANDLE)
		{
			vkDestroyImageView(_device, res.DepthView, nullptr);
			res.DepthView = VK_NULL_HANDLE;
		}
		if (res.View != VK_NULL_HANDLE)
		{
			vkDestroyImageView(_device, res.View, nullptr);
			res.View = VK_NULL_HANDLE;
		}
		if (res.Image != VK_NULL_HANDLE)
		{
			UseVmaDestroyImage(res.Image);
			res.Image = VK_NULL_HANDLE;
		}

		res.Level = nullptr;

	}

	void VulkanBase::_cleanup_active_offscreen_levels()
	{
		for (auto& res : _active_offscreen_levels)
		{
			_cleanup_offscreen_resource(res);
		}
		_active_offscreen_levels.clear();
	}

	bool VulkanBase::_cleanup_swap_chain()
	{
		// 1. 銷毀依賴於交換鏈視圖的 UI 幀緩衝 (Swapchain Framebuffers)
		for (auto framebuffer : _swap_chain_framebuffers)
		{
			if (framebuffer != VK_NULL_HANDLE)
			{
				vkDestroyFramebuffer(_device, framebuffer, nullptr);
			}
		}
		_swap_chain_framebuffers.clear();

		// 2. 銷毀交換鏈的圖像視圖 (Swapchain Image Views)
		for (auto imageView : _swap_chain_image_views)
		{
			if (imageView != VK_NULL_HANDLE)
			{
				vkDestroyImageView(_device, imageView, nullptr);
			}
		}
		_swap_chain_image_views.clear();

		// 3. 銷毀交換鏈本體 (Swapchain)
		// 注意：_swap_chain_images 是由 vkGetSwapchainImagesKHR 獲取的，不需要、也不能手動銷毀
		if (_swap_chain != VK_NULL_HANDLE)
		{
			vkDestroySwapchainKHR(_device, _swap_chain, nullptr);
			_swap_chain = VK_NULL_HANDLE;
		}
		_swap_chain_images.clear();

		return true;
	}

	void VulkanBase::_update_resizable_descriptor_sets()
	{}

#ifndef NDEBUG

	bool VulkanBase::_check_validation_layers()
	{
		uint32_t layerCount;
		if (VkResult result = vkEnumerateInstanceLayerProperties(&layerCount, nullptr))
		{
			INVENT_LOG_ERROR(std::format(" ERROR : [ VulkanBase ] Failed to get the count of instance layers! Error code: {}\n", int32_t(result)));
			return false;
		}
		if (layerCount)
		{
			std::vector<VkLayerProperties> availableLayers(layerCount);
			if (VkResult result = vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data()))
			{
				INVENT_LOG_ERROR(std::format("ERROR : [ VulkanBase ] Failed to enumerate instance layer properties! Error code: {}\n", int32_t(result)));
				return false;
			}
			bool found = false;
			for (auto& i : validationLayers)
			{
				for (auto& j : availableLayers)
					if (!strcmp(i, j.layerName))
					{
						found = true;
						break;
					}
				if (!found)
					i = nullptr;
			}
		}
		else
		{
			validationLayers.clear();
		}
		INVENT_LOG_INFO(std::format("INFO : Check instanceLayers done, Num : {}  :\n", validationLayers.size()));
		for (auto& name : validationLayers)
		{
			INVENT_LOG_INFO(std::format("\t {} \n", name));
		}
		return true;
	}

	bool VulkanBase::_setup_debug_messenger()
	{
		VkDebugUtilsMessengerCreateInfoEXT debugMsgInfo{};
		debugMsgInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugMsgInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugMsgInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugMsgInfo.pfnUserCallback = debugCallback;
		debugMsgInfo.pUserData = nullptr; // 自定义指针，直接传递到回调函数

		PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessenger =
			reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(_instance, "vkCreateDebugUtilsMessengerEXT"));
		if (vkCreateDebugUtilsMessenger)
		{
			if (vkCreateDebugUtilsMessenger(_instance, &debugMsgInfo, nullptr, &_debug_messenger) != VK_SUCCESS)
			{
				INVENT_LOG_ERROR(std::format("ERRORR : failed to set up debug messenger! \n"));
				return false;
			}
		}
		else
		{
			INVENT_LOG_ERROR(std::format("ERRORR : not found vkCreateDebugUtilsMessengerEXT \n"));
			return false;
		}
		return true;
	}

#endif // !NDEBUG


}

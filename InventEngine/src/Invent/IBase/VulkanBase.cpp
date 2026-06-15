#include "IEpch.h"
#include "VulkanBase.h"

#ifdef USE_VULKAN



#include "Invent/IEngineTools.h"

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

	constexpr uint32_t MAX_BINDLESS_TEXTURES = 200000;

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

		if (!Version_1_2_OrHigher())
		{
			AddDeviceExtension(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);
		}

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
		if (_render_pass_shadow != VK_NULL_HANDLE)
		{
			vkDestroyRenderPass(_device, _render_pass_shadow, nullptr);
			_render_pass_shadow = VK_NULL_HANDLE;
		}
		if (_render_pass_main != VK_NULL_HANDLE)
		{
			vkDestroyRenderPass(_device, _render_pass_main, nullptr);
			_render_pass_main = VK_NULL_HANDLE;
		}
		if (_render_pass_active_offscreen_levels != VK_NULL_HANDLE)
		{
			vkDestroyRenderPass(_device, _render_pass_active_offscreen_levels, nullptr);
			_render_pass_active_offscreen_levels = VK_NULL_HANDLE;
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
		auto isDeviceSuitable = [this](VkPhysicalDevice device)->bool {
			VkPhysicalDeviceProperties deviceProperties;
			VkPhysicalDeviceFeatures deviceFeatures;
			vkGetPhysicalDeviceProperties(device, &deviceProperties);
			vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

			_queue_family_indices = _find_queue_families(device);
			_physical_device_properties = deviceProperties;

			auto swapChainAdequate = [this, device]()->bool {
				_swap_chain_support = _query_swap_chain_support(device);
				return !_swap_chain_support.Formats.empty() && !_swap_chain_support.PresentModes.empty();
				};

			return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
				&& deviceFeatures.geometryShader
				&& deviceFeatures.samplerAnisotropy
				&& _queue_family_indices.IsComplete()
				&& _check_device_extension_support(device)
				&& _check_and_enable_bindless_feature(device)
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

		_find_max_hardware_textures();

		INVENT_LOG_INFO(std::format("[ VulkanBase ] device name : {} \n", _physical_device_properties.deviceName));
		INVENT_LOG_INFO(std::format("[ VulkanBase ] device maximum number of sampled image descriptors : {} \n", _max_hardware_textures));

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

		VkPhysicalDeviceVulkan12Features feat12{};
		feat12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
		feat12.descriptorBindingPartiallyBound = VK_TRUE;
		feat12.runtimeDescriptorArray = VK_TRUE;
		feat12.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
		feat12.descriptorBindingSampledImageUpdateAfterBind = VK_TRUE;

		if (Version_1_2_OrHigher())
		{
			feat11.pNext = &feat12;
		}
		else
		{
			_enabled_indexing_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
			_enabled_indexing_features.descriptorBindingSampledImageUpdateAfterBind = VK_TRUE;
			_enabled_indexing_features.pNext = nullptr;
			feat11.pNext = &_enabled_indexing_features;
			
		}

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

	bool VulkanBase::FindDepthFormat()
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
			_shadow_depth_format = _find_supported_format(
				{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D16_UNORM },
				VK_IMAGE_TILING_OPTIMAL,
				VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT
			);
			if (VK_FORMAT_UNDEFINED == _shadow_depth_format)
			{
				return false;
			}
			VkAttachmentDescription depth = {};
			depth.format = _shadow_depth_format;
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

			_render_pass_shadow = CreateRenderPass(attachments, subpasses, dependencies);
			if (VK_NULL_HANDLE == _render_pass_shadow)
			{
				return false;
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


			_render_pass_main = CreateRenderPass(attachments, subpasses, dependencies);
			if (VK_NULL_HANDLE == _render_pass_main)
			{
				return false;
			}

		}

		// postprocess & ui
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
					_render_pass_shadow,
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
					_render_pass_main,
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
					_render_pass_ui,
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

	bool VulkanBase::CreateBindlessDescriptorPool()
	{
		return _create_bindless_descriptor_pool();
	}

	bool VulkanBase::CreateGlobalPipelineLayout()
	{
		return _create_global_pipeline_layout();
	}

	bool VulkanBase::AllocaGlobalBindlessDescriptorSet()
	{
		return _alloca_global_bindless_descriptor_set();
	}

	bool VulkanBase::CreateCommandPool()
	{
		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = _queue_family_indices.GraphicsFamily;
		if (VkResult result = vkCreateCommandPool(_device, &poolInfo, nullptr, &_command_pool))
		{
			INVENT_LOG_ERROR(std::format("ERROR : [ VulkanBase ] Failed to create command pool! Error code: {}\n", int32_t(result)));
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

	VkDescriptorSetLayout VulkanBase::CreateDescriptorSetLayout(std::vector<VkDescriptorSetLayoutBinding>& bindings, bool is_bindless_set)
	{
		if (bindings.empty()) return VK_NULL_HANDLE;

		std::vector<VkDescriptorBindingFlags> bindingFlags(bindings.size());
		for (uint32_t i = 0; i < (uint32_t)bindings.size(); ++i)
		{
			if (is_bindless_set)
			{
				bindingFlags[i] = VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT |
					VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;
				if (i == bindings.size() - 1)
				{
					bindingFlags[i] |= VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT; // 在最后一个binding, shader-slang 中可不写死数量
				}
			}
			else
			{
				bindingFlags[i] = 0;
			}
		}

		VkDescriptorSetLayoutBindingFlagsCreateInfo layoutBindingFlagsInfo{};
		layoutBindingFlagsInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
		layoutBindingFlagsInfo.bindingCount = static_cast<uint32_t>(bindingFlags.size());
		layoutBindingFlagsInfo.pBindingFlags = bindingFlags.data();

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();
		layoutInfo.pNext = &layoutBindingFlagsInfo;
		if (is_bindless_set)
		{
			layoutInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
		}

		VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
		if (VkResult result = vkCreateDescriptorSetLayout(_device, &layoutInfo, nullptr, &descriptorSetLayout))
		{
			INVENT_LOG_ERROR(std::format(" [ VulkanBase ] Failed to create descriptor set layout! Error code: {}\n", int32_t(result)));
			return VK_NULL_HANDLE;
		}

		return descriptorSetLayout;
	}

	VkPipeline VulkanBase::CreateGraphicsPipeline(const GraphicsPipelineConfig& config)
	{
		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

		// shader 特化常数
		std::vector<VkSpecializationMapEntry> specMapEntries(2);
		specMapEntries[0].constantID = 0;
		specMapEntries[0].offset = offsetof(SpecializationData, BlendMode);
		specMapEntries[0].size = sizeof(int);
		specMapEntries[1].constantID = 1;
		specMapEntries[1].offset = offsetof(SpecializationData, PresetEffect);
		specMapEntries[1].size = sizeof(int);

		VkSpecializationInfo specInfo{};
		specInfo.mapEntryCount = config.SpecCount;
		specInfo.pMapEntries = specMapEntries.data();
		specInfo.dataSize = sizeof(SpecializationData);
		specInfo.pData = &config.SpecData;

		// 頂點階段
		VkPipelineShaderStageCreateInfo vertStageInfo{};
		vertStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertStageInfo.module = config.VertexShader;
		vertStageInfo.pName = "main";
		vertStageInfo.pSpecializationInfo = &specInfo;
		shaderStages.push_back(vertStageInfo);

		if (config.FragmentShader != VK_NULL_HANDLE)
		{
			VkPipelineShaderStageCreateInfo fragStageInfo{};
			fragStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			fragStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			fragStageInfo.module = config.FragmentShader;
			fragStageInfo.pName = "main";
			fragStageInfo.pSpecializationInfo = &specInfo;
			shaderStages.push_back(fragStageInfo);
		}

		// 頂點輸入狀態 (Manual Vertex Fetch 核心：保持留空)
		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		// 幾何拓撲
		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

		// 光柵化
		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = config.CullMode;
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizer.depthBiasEnable = (config.FragmentShader == VK_NULL_HANDLE) ? VK_TRUE : VK_FALSE;

		// 多重採樣
		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		// 深度測試狀態
		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = config.EnableDepthTest;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		if (config.BlendMode == ModelBlendMode::Translucent)
		{
			depthStencil.depthWriteEnable = VK_FALSE;
		}
		else
		{
			depthStencil.depthWriteEnable = config.EnableDepthTest;
		}

		// 顏色混合狀態
		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = 0xf;
		if (config.BlendMode == ModelBlendMode::Translucent)
		{
			colorBlendAttachment.blendEnable = VK_TRUE;
			colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
			colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
		}
		else
		{
			colorBlendAttachment.blendEnable = VK_FALSE;
		}

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.attachmentCount = (config.FragmentShader == VK_NULL_HANDLE) ? 0 : 1;
		colorBlending.pAttachments = &colorBlendAttachment;

		// 動態狀態
		std::array<VkDynamicState, 2> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();

		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		// 核心：Vulkan 1.3+ 動態渲染結構體設定
		VkPipelineRenderingCreateInfo pipelineRenderingCI{};
		pipelineRenderingCI.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
		if (config.FragmentShader != VK_NULL_HANDLE)
		{
			pipelineRenderingCI.colorAttachmentCount = 1;
			pipelineRenderingCI.pColorAttachmentFormats = &config.ColorAttachmentFormat;
		}
		else
		{
			pipelineRenderingCI.colorAttachmentCount = 0; // 陰影管線無顏色輸出
		}
		if (config.EnableDepthTest || config.FragmentShader == VK_NULL_HANDLE)
		{
			pipelineRenderingCI.depthAttachmentFormat = config.DepthAttachmentFormat;
		}
		else
		{
			pipelineRenderingCI.depthAttachmentFormat = VK_FORMAT_UNDEFINED;
		}

		// 總裝管線
		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
		pipelineInfo.pStages = shaderStages.data();
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = _global_pipeline_layout;
		if (Version_1_3_OrHigher())
		{
			pipelineInfo.pNext = &pipelineRenderingCI; // 1.3 核心：串接動態渲染格式
			pipelineInfo.renderPass = VK_NULL_HANDLE;  // 不需要傳入實體 RenderPass
		}
		else
		{
			pipelineInfo.pNext = nullptr;
			pipelineInfo.renderPass = config.RenderPass; // 舊版本依然走常規流程
			pipelineInfo.subpass = config.Subpass;
		}

		VkPipeline graphicsPipeline;
		if (VkResult result = vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline))
		{
			INVENT_LOG_ERROR(std::format(" [ VulkanBase ] Failed to create graphics pipeline! Error code: {}\n", int32_t(result)));
			return VK_NULL_HANDLE;
		}

		return graphicsPipeline;
	}

	VkShaderModule VulkanBase::CreateShaderMoudle(const std::string& path)
	{
		auto shaderCode = IEngineTools::ReadFile(path);

		if (shaderCode.size() % sizeof(uint32_t) != 0)
		{
			INVENT_LOG_ERROR(std::format(" [ VulkanBase ] [ CreateShaderMoudle ] read shader code error : {}", path));
			return VK_NULL_HANDLE;
		}

		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = shaderCode.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

		VkShaderModule shaderMoudle;
		if (VkResult result = vkCreateShaderModule(_device, &createInfo, nullptr, &shaderMoudle))
		{
			INVENT_LOG_ERROR(std::format(" [ VulkanBase ] [ CreateShaderMoudle ] Failed to create a shader module! Error code: {}\n", int32_t(result)));
			return VK_NULL_HANDLE;
		}

		return shaderMoudle;
	}

	void VulkanBase::DestroyShaderMoudle(VkShaderModule shader_moudle)
	{
		if (shader_moudle != VK_NULL_HANDLE)
			vkDestroyShaderModule(_device, shader_moudle, nullptr);
	}

	void VulkanBase::UpdateBindlessTextureSlot(uint32_t slot_id, VkImageView texture_image_view)
	{
		if (slot_id == 0 ||
			slot_id >= _current_descriptor_count ||
			texture_image_view == VK_NULL_HANDLE)
			return;

		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageView = texture_image_view;
		imageInfo.sampler = VK_NULL_HANDLE;
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkWriteDescriptorSet textureWrite{};
		textureWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		textureWrite.dstSet = _global_bindless_descriptor_set;
		textureWrite.dstBinding = 1;
		textureWrite.dstArrayElement = slot_id;
		textureWrite.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		textureWrite.descriptorCount = 1;
		textureWrite.pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(_device, 1, &textureWrite, 0, nullptr);

	}

	bool VulkanBase::CreateSyncObjects(std::vector<VkFence>& frameFence, std::vector<VkSemaphore>& acquireSemaphores, std::vector<VkSemaphore>& submitSemaphores)
	{
		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // 初始状态为已信号，避免第一次等待时死锁

		frameFence.resize(MAX_FRAMES_IN_FLIGHT);
		for (unsigned int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			if (VkResult result = vkCreateFence(_device, &fenceInfo, nullptr, &frameFence[i]))
			{
				INVENT_LOG_ERROR(std::format(" [ VulkanBase ] Failed to create fences! Error codes: {},\n", int32_t(result)));
				return false;
			}
		}
		acquireSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		for (unsigned int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			if (VkResult result = vkCreateSemaphore(_device, &semaphoreInfo, nullptr, &acquireSemaphores[i]))
			{
				INVENT_LOG_ERROR(std::format(" [ VulkanBase ] Failed to create _acquire_semaphores! Error codes: {},\n", int32_t(result)));
				return false;
			}
		}
		submitSemaphores.resize(_swap_chain_image_count);
		for (unsigned int i = 0; i < _swap_chain_image_count; ++i)
		{
			if (VkResult result = vkCreateSemaphore(_device, &semaphoreInfo, nullptr, &submitSemaphores[i]))
			{
				INVENT_LOG_ERROR(std::format(" [ VulkanBase ] Failed to create _submit_semaphores! Error codes: {},\n", int32_t(result)));
				return false;
			}
		}

		return true;
	}

	bool VulkanBase::CreateCommandBuffers(std::vector<VkCommandBuffer>& buffers)
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = _command_pool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

		buffers.resize(MAX_FRAMES_IN_FLIGHT);
		if (VkResult result = vkAllocateCommandBuffers(_device, &allocInfo, buffers.data()))
		{
			INVENT_LOG_ERROR(std::format(" [ VulkanBase ] Failed to allocate command buffers! Error code: {}\n", int32_t(result)));
			return false;
		}

		return false;
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

	bool VulkanBase::RequestOffscreenLevels(const std::vector<void*>& required_levels)
	{
		_cleanup_active_offscreen_levels();
		if (_render_pass_active_offscreen_levels != VK_NULL_HANDLE)
		{
			vkDestroyRenderPass(_device, _render_pass_active_offscreen_levels, nullptr);
			_render_pass_active_offscreen_levels = VK_NULL_HANDLE;
		}

		if (required_levels.empty())
			return true;

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

			_render_pass_active_offscreen_levels = CreateRenderPass(attachments, subpasses, dependencies);

			if (VK_NULL_HANDLE == _render_pass_active_offscreen_levels)
			{
				INVENT_LOG_ERROR(std::format(" [ VulkanBase ] [ func RequestOffscreenLevels ] Failed to create render pass.\n"));
				return false;
			}
		}

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
				
				VkImageView viewAttachments[] = { newLevel.View, newLevel.DepthView };

				newLevel.Framebuffer = CreateFramebuffer(OffscreenLevelSizeX,
					OffscreenLevelSizeY,
					_render_pass_active_offscreen_levels,
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
		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		if (extensionCount > 0)
		{

			if (VkResult result = vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data()))
			{
				INVENT_LOG_ERROR(std::format(" [ VulkanBase ] Failed to enumerate device extension properties! Error code: {}\n", int32_t(result)));
				return false;
			}
		}
		// 1. 將所有可用的擴展名字放入集合（Set）中，方便 O(1) 快速查找
		std::unordered_set<std::string> availableNameSet;
		for (const auto& ext : availableExtensions)
		{
			availableNameSet.insert(ext.extensionName);
		}
		// 2. 建立一個臨時陣列，只保留確定支援的擴展
		std::vector<const char*> validExtensions;
		bool allRequiredSupported = true;

		for (const auto& requiredExt : deviceExtensions)
		{
			if (requiredExt == nullptr) continue;

			if (availableNameSet.count(requiredExt) > 0)
			{
				validExtensions.push_back(requiredExt);
			}
			else
			{
				INVENT_LOG_ERROR(std::format(" [ VulkanBase ] Required extension NOT supported: {}\n", requiredExt));
				allRequiredSupported = false;
			}
		}

		deviceExtensions = validExtensions;

		INVENT_LOG_INFO(std::format(" Check deviceExtensions done, Num : {}  :\n", deviceExtensions.size()));
		for (auto& name : deviceExtensions)
		{
			INVENT_LOG_INFO(std::format("\t {} \n", name));
		}
		return allRequiredSupported;
	}

	bool VulkanBase::_check_and_enable_bindless_feature(VkPhysicalDevice device)
	{
		if (Version_1_2_OrHigher()) return true;

		//  如果是 Vulkan 1.0 或 1.1，必須手動透過 pNext 鏈條向硬體查詢擴展特性
		VkPhysicalDeviceDescriptorIndexingFeaturesEXT indexingFeatures{};
		indexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
		

		VkPhysicalDeviceFeatures2 deviceFeatures2{};
		deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		deviceFeatures2.pNext = &indexingFeatures;

		// 從選好的物理設備中查詢硬體支援情況
		vkGetPhysicalDeviceFeatures2(device, &deviceFeatures2);


		// 嚴格驗證 Shader-slang 和 Bindless 架構必須的三個基礎特性
		bool hasPartiallyBound = (indexingFeatures.descriptorBindingPartiallyBound == VK_TRUE);
		bool hasRuntimeArray = (indexingFeatures.runtimeDescriptorArray == VK_TRUE);
		bool hasNonUniformIndex = (indexingFeatures.shaderSampledImageArrayNonUniformIndexing == VK_TRUE);

		if (!hasPartiallyBound || !hasRuntimeArray || !hasNonUniformIndex)
		{
			return false;
		}

		_enabled_indexing_features = indexingFeatures;

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

	bool VulkanBase::_create_bindless_descriptor_pool()
	{
		_current_descriptor_count = std::min(MAX_BINDLESS_TEXTURES, _max_hardware_textures);
		INVENT_LOG_INFO(std::format("[ VulkanBase ] Current descriptor count : {} \n", _current_descriptor_count));

		VkDescriptorPoolSize poolSizes[1];
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[0].descriptorCount = _current_descriptor_count;

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.pNext = nullptr;
		poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
		poolInfo.maxSets = 1;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = poolSizes;

		if (VkResult result = vkCreateDescriptorPool(_device, &poolInfo, nullptr, &_bindless_descriptor_pool))
		{
			INVENT_LOG_ERROR(std::format("ERROR : [ VulkanBase ] Failed to create descriptor pool! Error code: {}\n", int32_t(result)));
			return false;
		}

		return true;

	}

	void VulkanBase::_find_max_hardware_textures()
	{
		VkPhysicalDeviceDescriptorIndexingProperties indexingProps{};
		indexingProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES;

		VkPhysicalDeviceProperties2 deviceProps2{};
		deviceProps2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
		deviceProps2.pNext = &indexingProps;

		vkGetPhysicalDeviceProperties2(_physical_device, &deviceProps2);

		_max_hardware_textures = indexingProps.maxPerStageDescriptorUpdateAfterBindSampledImages;
		
	}

	bool VulkanBase::_create_global_pipeline_layout()
	{
		// Set0 
		// binding 0 : ubo
		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr;
		// binding 1 : point light ssbo
		VkDescriptorSetLayoutBinding pointLightSSBOLayoutBinding{};
		uboLayoutBinding.binding = 1;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr;
		std::vector<VkDescriptorSetLayoutBinding> set0Bindings = {
			uboLayoutBinding,
			pointLightSSBOLayoutBinding
		};
		auto set0Layout = CreateDescriptorSetLayout(set0Bindings);

		// Set1 Bindless Layout and Material SSBO
		// binding 0 : 全局采样器
		VkDescriptorSetLayoutBinding globalSamplerLayoutBinding{};
		globalSamplerLayoutBinding.binding = 0;
		globalSamplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
		globalSamplerLayoutBinding.descriptorCount = 1;
		globalSamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		globalSamplerLayoutBinding.pImmutableSamplers = nullptr;
		// binding 1 : 材质 SSBO
		VkDescriptorSetLayoutBinding materialSSBOLayoutBinding{};
		materialSSBOLayoutBinding.binding = 1;
		materialSSBOLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		materialSSBOLayoutBinding.descriptorCount = 1;
		materialSSBOLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		materialSSBOLayoutBinding.pImmutableSamplers = nullptr;
		// binding 2 : 贴图阵列
		VkDescriptorSetLayoutBinding textureListLayoutBinding{};
		textureListLayoutBinding.binding = 2;
		textureListLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		textureListLayoutBinding.descriptorCount = _current_descriptor_count;
		textureListLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		textureListLayoutBinding.pImmutableSamplers = nullptr;
		std::vector<VkDescriptorSetLayoutBinding> set1Bindings = {
			globalSamplerLayoutBinding,
			materialSSBOLayoutBinding,
			textureListLayoutBinding
		};
		auto set1Layout = CreateDescriptorSetLayout(set1Bindings, true);

		// Set2 几何 SSBO
		// binding 0 : ssbo
		VkDescriptorSetLayoutBinding vertexesSSBOLayoutBinding{};
		vertexesSSBOLayoutBinding.binding = 0;
		vertexesSSBOLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		vertexesSSBOLayoutBinding.descriptorCount = 1;
		vertexesSSBOLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		vertexesSSBOLayoutBinding.pImmutableSamplers = nullptr;
		std::vector<VkDescriptorSetLayoutBinding> set2Bindings = {
			vertexesSSBOLayoutBinding
		};
		auto set2Layout = CreateDescriptorSetLayout(set2Bindings);

		// Set3 UI vertex ssbo
		// binding 0 : ssbo
		VkDescriptorSetLayoutBinding UISSBOLayoutBinding{};
		UISSBOLayoutBinding.binding = 0;
		UISSBOLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		UISSBOLayoutBinding.descriptorCount = 1;
		UISSBOLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		UISSBOLayoutBinding.pImmutableSamplers = nullptr;
		std::vector<VkDescriptorSetLayoutBinding> set3Bindings = {
			UISSBOLayoutBinding
		};
		auto set3Layout = CreateDescriptorSetLayout(set3Bindings);

		_descriptor_set_layouts = {
			set0Layout,
			set1Layout,
			set2Layout,
			set3Layout
		};

		// 配置全域 Push Constants (用來傳遞當前繪製的 MeshID 與頂點偏移)
		VkPushConstantRange pushRange{};
		pushRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushRange.offset = 0;
		pushRange.size = sizeof(uint32_t) * 2; // 傳入 2 個 uint32 (MeshID, BaseVertexOffset)

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(_descriptor_set_layouts.size());
		pipelineLayoutInfo.pSetLayouts = _descriptor_set_layouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushRange;

		if (VkResult result = vkCreatePipelineLayout(_device, &pipelineLayoutInfo, nullptr, &_global_pipeline_layout))
		{
			INVENT_LOG_ERROR(std::format("ERROR : [ VulkanBase ] Failed to create global pipeline layout! Error code: {}\n", int32_t(result)));
			return false;
		}

		return true;
	}

	bool VulkanBase::_alloca_global_bindless_descriptor_set()
	{
		// 1. 設置變長度陣列的實際分配數量
		VkDescriptorSetVariableDescriptorCountAllocateInfo variableCountInfo{};
		variableCountInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO;
		variableCountInfo.descriptorSetCount = 1;
		variableCountInfo.pDescriptorCounts = &_current_descriptor_count;

		// 2. 填充常規的 Allocate 資訊
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = _bindless_descriptor_pool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &_descriptor_set_layouts[1]; // set1Layout (Bindless)
		allocInfo.pNext = &variableCountInfo;

		if (VkResult result = vkAllocateDescriptorSets(_device, &allocInfo, &_global_bindless_descriptor_set))
		{
			INVENT_LOG_ERROR(std::format("ERROR : [ VulkanBase ] Failed to create global descriptor set! Error code: {}\n", int32_t(result)));
			return false;
		}

		// white texture is 0

		return true;
	}

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


#endif // USE_VULKAN

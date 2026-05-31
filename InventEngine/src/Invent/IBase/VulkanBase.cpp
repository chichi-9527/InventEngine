#include "IEpch.h"
#include "VulkanBase.h"

namespace INVENT
{
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

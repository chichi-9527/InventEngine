#include "IEpch.h"
#include "IVulkanRenderer.h"

#ifdef USE_VULKAN

#include "Invent/IBase/VulkanBase.h"
#include "Invent/ITexture/IVulkanTexture.h"

#define CREATE_VERTEX_SHADER(x) auto x##VertexShader = VulkanBase::Base().CreateShaderMoudle(x##VertexShaderPath)
#define DESTROY_VERTEX_SHADER(x) VulkanBase::Base().DestroyShaderMoudle(x##VertexShader); x##VertexShader = VK_NULL_HANDLE
#define CREATE_FRAGMENT_SHADER(x) auto x##FragmentShader = VulkanBase::Base().CreateShaderMoudle(x##FragmentShaderPath)
#define DESTROY_FRAGMENT_SHADER(x) VulkanBase::Base().DestroyShaderMoudle(x##FragmentShader); x##FragmentShader = VK_NULL_HANDLE

namespace INVENT
{

	constexpr uint32_t OffscreenLevelSizeX = 1024;
	constexpr uint32_t OffscreenLevelSizeY = 1024;
	constexpr uint32_t TempLevelDepthSizeX = 1024;
	constexpr uint32_t TempLevelDepthSizeY = 1024;
	constexpr uint32_t ShadowMapSizeX = 2048;
	constexpr uint32_t ShadowMapSizeY = 2048;

	constexpr const char* ShadowVertexShaderPath = "";
	constexpr const char* ShadowFragmentShaderPath = "";
	constexpr const char* OffscreenVertexShaderPath = "";
	constexpr const char* OffscreenFragmentShaderPath = "";
	constexpr const char* MainMeshVertexShaderPath = "";
	constexpr const char* MainMeshFragmentShaderPath = "";
	constexpr const char* PostprocessVertexShaderPath = "";
	constexpr const char* PostprocessFragmentShaderPath = "";
	constexpr const char* UIVertexShaderPath = "";
	constexpr const char* UIFragmentShaderPath = "";

	constexpr IVulkanTexture2DManagement::Texture2DHandle WhiteHandle = IVulkanTexture2DManagement::GetWhitePixel();
	constexpr IVulkanTexture2DManagement::Texture2DHandle BlackHandle = IVulkanTexture2DManagement::GetBlackPixel();
	constexpr IVulkanTexture2DManagement::Texture2DHandle NormalHandle = IVulkanTexture2DManagement::GetNormalPixel();

	static uint32_t ImageIndex = 0;

	bool IVulkanRenderer::Init()
	{
		return _init_pipelines() &&
			_create_commnad_buffers() &&
			_create_sync_objects();
	}

	void IVulkanRenderer::Shutdown()
	{

	}

	void IVulkanRenderer::BeginRender(const ICamera* camera)
	{

	}

	bool IVulkanRenderer::WaitForFence(uint32_t frame_index)
	{
		if (VkResult result = vkWaitForFences(VulkanBase::Base().GetDevice(),
			1,
			&_frame_fences[frame_index],
			VK_TRUE, UINT64_MAX))
		{
			INVENT_LOG_ERROR(std::format("ERROR : [VulkanBase] vkWaitForFences error; frame index : {}\n", frame_index));
			return false;
		}
		return true;
	}

	int IVulkanRenderer::AcquireNextImage(uint32_t frame_index)
	{
		VkResult result = vkAcquireNextImageKHR(VulkanBase::Base().GetDevice(), 
			VulkanBase::Base().GetSwapChain(), 
			UINT64_MAX, 
			_acquire_semaphores[frame_index], 
			VK_NULL_HANDLE, 
			&ImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			if (!VulkanBase::Base().RecreateSwapChain())
			{
				return -2;
			}
			return -1;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			INVENT_LOG_ERROR(std::format(" [VulkanBase] failed to acquire swap chain image! \n"));
			return -2;
		}

		// todo update ubo

		// reset fences
		if (VkResult result = vkResetFences(VulkanBase::Base().GetDevice(),
			1, 
			&_frame_fences[frame_index]))
		{
			INVENT_LOG_ERROR(std::format(" [VulkanBase] vkResetFences error : {}\n", (int32_t)result));
		}

		// reset command buffer
		vkResetCommandBuffer(_command_buffers[frame_index], 0);

		return 0;
	}

	bool IVulkanRenderer::ToRenderScence(uint32_t frame_index)
	{
		return false;
	}

	bool IVulkanRenderer::_init_pipelines()
	{
		// shadow
		CREATE_VERTEX_SHADER(Shadow);
		// 不透明
		{
			VulkanBase::GraphicsPipelineConfig pipelineConfig{};
			pipelineConfig.VertexShader = ShadowVertexShader;
			if (VulkanBase::Base().Version_1_3_OrHigher())
			{
				pipelineConfig.DepthAttachmentFormat = VulkanBase::Base().GetShadowDepthFormat();
				pipelineConfig.ColorAttachmentFormat = VK_FORMAT_UNDEFINED;
			}
			else
			{
				pipelineConfig.RenderPass = VulkanBase::Base().GetShadowRenderPass();
			}
			pipelineConfig.CullMode = VK_CULL_MODE_FRONT_BIT;
			IVulkanRenderer::_shadow_pipeline_opaque = VulkanBase::Base().CreateGraphicsPipeline(pipelineConfig);
		}
		// 镂空或半透明
		CREATE_FRAGMENT_SHADER(Shadow);
		{
			VulkanBase::GraphicsPipelineConfig pipelineConfig{};
			pipelineConfig.VertexShader = ShadowVertexShader;
			pipelineConfig.FragmentShader = ShadowFragmentShader;
			pipelineConfig.BlendMode = VulkanBase::ModelBlendMode::Masked;
			pipelineConfig.SpecData.BlendMode = 1;
			if (VulkanBase::Base().Version_1_3_OrHigher())
			{
				pipelineConfig.DepthAttachmentFormat = VulkanBase::Base().GetShadowDepthFormat();
				pipelineConfig.ColorAttachmentFormat = VK_FORMAT_UNDEFINED;
			}
			else
			{
				pipelineConfig.RenderPass = VulkanBase::Base().GetShadowRenderPass();
			}
			pipelineConfig.CullMode = VK_CULL_MODE_FRONT_BIT;
			IVulkanRenderer::_shadow_pipeline_masked = VulkanBase::Base().CreateGraphicsPipeline(pipelineConfig);
		}
		DESTROY_VERTEX_SHADER(Shadow);
		DESTROY_FRAGMENT_SHADER(Shadow);

		// offscreen
		CREATE_VERTEX_SHADER(Offscreen);
		CREATE_FRAGMENT_SHADER(Offscreen);
		// 不透明
		{
			VulkanBase::GraphicsPipelineConfig pipelineConfig{};
			pipelineConfig.VertexShader = OffscreenVertexShader;
			pipelineConfig.FragmentShader = OffscreenFragmentShader;
			// will 1
			pipelineConfig.SpecCount = 2;
			if (VulkanBase::Base().Version_1_3_OrHigher())
			{
				pipelineConfig.ColorAttachmentFormat = VK_FORMAT_B8G8R8A8_UNORM;
				pipelineConfig.DepthAttachmentFormat = VulkanBase::Base().GetDepthFormat();
			}
			else
			{
				pipelineConfig.RenderPass = VulkanBase::Base().GetOffscreenRenderPass();
			}
			pipelineConfig.CullMode = VK_CULL_MODE_BACK_BIT;
			IVulkanRenderer::_offscreen_opaque_pipeline = VulkanBase::Base().CreateGraphicsPipeline(pipelineConfig);
		}
		// 镂空 || 半透明
		{
			VulkanBase::GraphicsPipelineConfig pipelineConfig{};
			pipelineConfig.VertexShader = OffscreenVertexShader;
			pipelineConfig.FragmentShader = OffscreenFragmentShader;
			// will 1
			pipelineConfig.SpecData = { 1,0 };
			pipelineConfig.SpecCount = 2;
			pipelineConfig.BlendMode = VulkanBase::ModelBlendMode::Masked;
			if (VulkanBase::Base().Version_1_3_OrHigher())
			{
				pipelineConfig.ColorAttachmentFormat = VK_FORMAT_B8G8R8A8_UNORM;
				pipelineConfig.DepthAttachmentFormat = VulkanBase::Base().GetDepthFormat();
			}
			else
			{
				pipelineConfig.RenderPass = VulkanBase::Base().GetOffscreenRenderPass();
			}
			pipelineConfig.CullMode = VK_CULL_MODE_BACK_BIT;
			IVulkanRenderer::_offscreen_masked_pipeline = VulkanBase::Base().CreateGraphicsPipeline(pipelineConfig);
		}
		// 透明
		{
			VulkanBase::GraphicsPipelineConfig pipelineConfig{};
			pipelineConfig.VertexShader = OffscreenVertexShader;
			pipelineConfig.FragmentShader = OffscreenFragmentShader;
			// will 1
			pipelineConfig.SpecData = { 2,0 };
			pipelineConfig.SpecCount = 2;
			pipelineConfig.BlendMode = VulkanBase::ModelBlendMode::Translucent;
			if (VulkanBase::Base().Version_1_3_OrHigher())
			{
				pipelineConfig.ColorAttachmentFormat = VK_FORMAT_B8G8R8A8_UNORM;
				pipelineConfig.DepthAttachmentFormat = VulkanBase::Base().GetDepthFormat();
			}
			else
			{
				pipelineConfig.RenderPass = VulkanBase::Base().GetOffscreenRenderPass();
			}
			pipelineConfig.CullMode = VK_CULL_MODE_BACK_BIT;
			IVulkanRenderer::_offscreen_translucent_pipeline = VulkanBase::Base().CreateGraphicsPipeline(pipelineConfig);
		}
		DESTROY_VERTEX_SHADER(Offscreen);
		DESTROY_FRAGMENT_SHADER(Offscreen);

		// main
		CREATE_VERTEX_SHADER(MainMesh);
		CREATE_FRAGMENT_SHADER(MainMesh);
		// 不透明
		{
			VulkanBase::GraphicsPipelineConfig pipelineConfig{};
			pipelineConfig.VertexShader = MainMeshVertexShader;
			pipelineConfig.FragmentShader = MainMeshFragmentShader;
			if (VulkanBase::Base().Version_1_3_OrHigher())
			{
				pipelineConfig.ColorAttachmentFormat = VK_FORMAT_B8G8R8A8_UNORM;
				pipelineConfig.DepthAttachmentFormat = VulkanBase::Base().GetDepthFormat();
			}
			else
			{
				pipelineConfig.RenderPass = VulkanBase::Base().GetOffscreenRenderPass();
			}
			pipelineConfig.CullMode = VK_CULL_MODE_BACK_BIT;
			IVulkanRenderer::_main_opaque_pipeline = VulkanBase::Base().CreateGraphicsPipeline(pipelineConfig);
		}
		// 镂空 || 半透明
		{
			VulkanBase::GraphicsPipelineConfig pipelineConfig{};
			pipelineConfig.VertexShader = MainMeshVertexShader;
			pipelineConfig.FragmentShader = MainMeshFragmentShader;
			pipelineConfig.SpecData = { 1,0 };
			pipelineConfig.BlendMode = VulkanBase::ModelBlendMode::Masked;
			if (VulkanBase::Base().Version_1_3_OrHigher())
			{
				pipelineConfig.ColorAttachmentFormat = VK_FORMAT_B8G8R8A8_UNORM;
				pipelineConfig.DepthAttachmentFormat = VulkanBase::Base().GetDepthFormat();
			}
			else
			{
				pipelineConfig.RenderPass = VulkanBase::Base().GetOffscreenRenderPass();
			}
			pipelineConfig.CullMode = VK_CULL_MODE_BACK_BIT;
			IVulkanRenderer::_main_masked_pipeline = VulkanBase::Base().CreateGraphicsPipeline(pipelineConfig);
		}
		// 透明
		{
			VulkanBase::GraphicsPipelineConfig pipelineConfig{};
			pipelineConfig.VertexShader = MainMeshVertexShader;
			pipelineConfig.FragmentShader = MainMeshFragmentShader;
			pipelineConfig.SpecData = { 2,0 };
			pipelineConfig.BlendMode = VulkanBase::ModelBlendMode::Translucent;
			if (VulkanBase::Base().Version_1_3_OrHigher())
			{
				pipelineConfig.ColorAttachmentFormat = VK_FORMAT_B8G8R8A8_UNORM;
				pipelineConfig.DepthAttachmentFormat = VulkanBase::Base().GetDepthFormat();
			}
			else
			{
				pipelineConfig.RenderPass = VulkanBase::Base().GetOffscreenRenderPass();
			}
			pipelineConfig.CullMode = VK_CULL_MODE_BACK_BIT;
			IVulkanRenderer::_main_translucent_pipeline = VulkanBase::Base().CreateGraphicsPipeline(pipelineConfig);
		}
		DESTROY_VERTEX_SHADER(MainMesh);
		DESTROY_FRAGMENT_SHADER(MainMesh);

		// postprocess
		CREATE_VERTEX_SHADER(Postprocess);
		CREATE_FRAGMENT_SHADER(Postprocess);
		{
			VulkanBase::GraphicsPipelineConfig pipelineConfig{};
			pipelineConfig.VertexShader = PostprocessVertexShader;
			pipelineConfig.FragmentShader = PostprocessFragmentShader;
			pipelineConfig.SpecCount = 0;
			if (VulkanBase::Base().Version_1_3_OrHigher())
			{
				pipelineConfig.ColorAttachmentFormat = VulkanBase::Base().GetSwapChainImageFormat();
				pipelineConfig.DepthAttachmentFormat = VK_FORMAT_UNDEFINED;
			}
			else
			{
				pipelineConfig.RenderPass = VulkanBase::Base().GetUiRenderPass();
			}
			pipelineConfig.EnableDepthTest = VK_FALSE;
			pipelineConfig.CullMode = VK_CULL_MODE_BACK_BIT;
			IVulkanRenderer::_postprocess_pipeline = VulkanBase::Base().CreateGraphicsPipeline(pipelineConfig);
		}
		DESTROY_VERTEX_SHADER(Postprocess);
		DESTROY_FRAGMENT_SHADER(Postprocess);

		// ui
		CREATE_VERTEX_SHADER(UI);
		CREATE_FRAGMENT_SHADER(UI);
		{
			VulkanBase::GraphicsPipelineConfig pipelineConfig{};
			pipelineConfig.VertexShader = UIVertexShader;
			pipelineConfig.FragmentShader = UIFragmentShader;
			pipelineConfig.BlendMode = VulkanBase::ModelBlendMode::Translucent;
			if (VulkanBase::Base().Version_1_3_OrHigher())
			{
				pipelineConfig.ColorAttachmentFormat = VulkanBase::Base().GetSwapChainImageFormat();
				pipelineConfig.DepthAttachmentFormat = VK_FORMAT_UNDEFINED;
			}
			else
			{
				pipelineConfig.RenderPass = VulkanBase::Base().GetUiRenderPass();
			}
			pipelineConfig.EnableDepthTest = VK_FALSE;
			pipelineConfig.CullMode = VK_CULL_MODE_NONE;
			IVulkanRenderer::_ui_pipeline = VulkanBase::Base().CreateGraphicsPipeline(pipelineConfig);
		}
		DESTROY_VERTEX_SHADER(UI);
		DESTROY_FRAGMENT_SHADER(UI);

		return true;
	}

	bool IVulkanRenderer::_create_sync_objects()
	{
		return VulkanBase::Base().CreateSyncObjects(_frame_fences, _acquire_semaphores, _submit_semaphores);
	}

	bool IVulkanRenderer::_create_commnad_buffers()
	{
		return VulkanBase::Base().CreateCommandBuffers(_command_buffers);
	}

	void IVulkanRenderer::_rendering_shadow_opaque()
	{
		
	}


}


#endif // USE_VULKAN

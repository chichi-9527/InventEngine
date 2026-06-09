#include "IEpch.h"
#include "IVulkanRenderer.h"

#include "Invent/IBase/VulkanBase.h"

constexpr const char* ShadowVertexShaderPath = "";

namespace INVENT
{

	void IVulkanRenderer::Init()
	{

	}

	bool IVulkanRenderer::InitPipelines()
	{
		// shadow
		{
			VulkanBase::GraphicsPipelineConfig pipelineConfig{};
			pipelineConfig.VertexShader = VulkanBase::Base().CreateShaderMoudle(ShadowVertexShaderPath);
			// TODO

			if (VulkanBase::Base().Version_1_3_OrHigher())
			{

			}
			else
			{
				pipelineConfig.RenderPass = VulkanBase::Base().GetShadowRenderPass();
			}

			IVulkanRenderer::_shadow_pipeline = VulkanBase::Base().CreateGraphicsPipeline(pipelineConfig);

			// destroy
			VulkanBase::Base().DestroyShaderMoudle(pipelineConfig.VertexShader);

		}
		


		return true;
	}


}

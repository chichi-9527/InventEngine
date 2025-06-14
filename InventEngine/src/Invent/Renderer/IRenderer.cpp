#include "IEpch.h"
#include "IRenderer.h"

namespace INVENT
{
	std::unique_ptr<IRenderer::SceneData> IRenderer::_sceme_data = std::make_unique<IRenderer::SceneData>();

	void IRenderer::Init()
	{
		IRendererCommend::Init();
	}

	void IRenderer::Shutdown()
	{}

	void IRenderer::BeginRender(const ICamera* camera)
	{
	
	}

	void IRenderer::EndRender()
	{}

	void IRenderer::Submit()
	{}
}
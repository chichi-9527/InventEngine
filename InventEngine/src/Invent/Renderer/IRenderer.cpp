#include "IEpch.h"
#include "IRenderer.h"

namespace INVENT
{

	void IRenderer::Init()
	{
		IRendererCommend::Init();
	}

	void IRenderer::Shutdown()
	{}

	void IRenderer::BeginRender()
	{}

	void IRenderer::EndRender()
	{}

	void IRenderer::Submit()
	{}
}
#include "IEpch.h"
#include "IRenderer.h"

#include "IRenderer2D.h"


#define INVENT_MAX_VERTEX_RENDER_ONCE 20000 * 6
#define INVENT_MAX_INDEX_RENDER_ONCE INVENT_MAX_VERTEX_RENDER_ONCE
#define INVENT_MAX_TEXTURE_RENDER_ONCE 32

namespace INVENT
{
	std::unique_ptr<IRenderer::SceneData> IRenderer::_scene_data = std::make_unique<IRenderer::SceneData>();

	void IRenderer::Init()
	{
		IRendererCommend::Init();
		IRenderer2D::Init();
	}

	void IRenderer::Shutdown()
	{
		IRenderer2D::Shutdown();
	}

	void IRenderer::BeginRender(const ICamera* camera)
	{
		if (camera)
			_scene_data->ViewProjectionMatrix = camera->GetViewProjectionMatrix();
		_scene_data->ViewProjectionMatrix = glm::mat4(1.0f);
	}

	void IRenderer::EndRender()
	{}

	void IRenderer::Submit(const IShader* shader, const std::shared_ptr<IVertexArray> vertex_array, const glm::mat4& transfrom)
	{
		shader->Bind();
		shader->SetMat4("u_ViewProjection", _scene_data->ViewProjectionMatrix);
		shader->SetMat4("u_Transfrom", transfrom);

		IRendererCommend::DrawIndexed(vertex_array, vertex_array->GetIndexBuffer()->GetCount());
	}
}
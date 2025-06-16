#include "IEpch.h"
#include "IRenderer.h"

namespace INVENT
{
	std::unique_ptr<IRenderer::SceneData> IRenderer::_scene_data = std::make_unique<IRenderer::SceneData>();

	void IRenderer::Init()
	{
		IRendererCommend::Init();
	}

	void IRenderer::Shutdown()
	{}

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

		IRendererCommend::DrawIndexed(vertex_array, vertex_array->GetIndexBuffer()->GetCount());
	}
}
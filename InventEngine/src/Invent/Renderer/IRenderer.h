#ifndef _IRENDERER_
#define _IRENDERER_

#include "IRendererCommand.h"
#include "IVertexArray.h"

#include "IBase/ICamera.h"

#include "Shader/IShader.h"

#include <memory>

namespace INVENT
{
	class IMesh;
	class IRenderer 
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginRender(const ICamera* camera);
		static void EndRender();

		static void Submit(const IShader* shader, const std::shared_ptr<IVertexArray> vertex_array, const glm::mat4& transfrom = glm::mat4(1.0f));

		static void StartARender();
		static void NextARender();
		static void Rendering();

		static void DrawMesh(IMesh* mesh, const glm::mat4& model_martrix);

	private:
		static void _init_renderer_data();
		static void _free_renderer_data();

	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static std::unique_ptr<SceneData> _scene_data;
	};
}

#endif // !_IRENDERER_


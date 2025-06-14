#ifndef _IRENDERER_
#define _IRENDERER_

#include "IRendererCommand.h"

#include "IBase/ICamera.h"

#include <memory>

namespace INVENT
{
	class IRenderer 
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginRender(const ICamera* camera);
		static void EndRender();

		static void Submit();

	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static std::unique_ptr<SceneData> _sceme_data;
	};
}

#endif // !_IRENDERER_


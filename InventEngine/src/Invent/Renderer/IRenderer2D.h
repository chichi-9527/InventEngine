#ifndef _IRENDERER2D_
#define _IRENDERER2D_

#include "IBase/ICamera.h"

#include "2D/ISquare2dActor.h"

namespace INVENT
{

	class IRenderer2D 
	{
	public:
		static void Init();
		static void Init(float line_width);
		static void Shutdown();

		static void BeginRender(const ICamera* camera);
		static void EndRender();

		static void DrawSquare(ISquare2dActor* actor);
		static void DrawString(const std::string& string, const glm::vec4& color, const glm::vec2& position, float scale);
		static void DrawWString(const std::wstring& wstring, const glm::vec4& color, const glm::vec2& position, float scale);

	private:
		static void StartARender();
		static void NextARender();
		static void Rendering();
	};
}

#endif // !_IRENDERER2D_

#ifndef _IRENDERER2D_
#define _IRENDERER2D_

#include "Invent/IBase/ICamera.h"

#include "Invent/2D/ISquare2dActor.h"

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

		/*
		* model 0: 使用绝对 position (0~window.size); 1: 使用相对 position (0~1)
		*/
		static void DrawString(const std::string& string, const glm::vec4& color, const glm::vec2& position, float px, unsigned int model);
		static void DrawWString(const std::wstring& wstring, const glm::vec4& color, const glm::vec2& position, float px, unsigned int model);
		static void DrawString(const std::string& string, const glm::vec4& color, const glm::vec2& position, float px, unsigned int index, unsigned int model);
		static void DrawWString(const std::wstring& wstring, const glm::vec4& color, const glm::vec2& position, float px, unsigned int index, unsigned int model);
		
		static void DrawString(const std::string& string, const glm::vec4& color, const glm::vec3& position, const glm::vec3& rotation, float px);
		static void DrawWString(const std::wstring& wstring, const glm::vec4& color, const glm::vec3& position, const glm::vec3& rotation, float px);
		static void DrawString(const std::string& string, const glm::vec4& color, const glm::vec3& position, const glm::vec3& rotation, float px, unsigned int index);
		static void DrawWString(const std::wstring& wstring, const glm::vec4& color, const glm::vec3& position, const glm::vec3& rotation, float px, unsigned int index);

	private:
		static void StartARender();
		static void NextARender();
		static void Rendering();
	};
}

#endif // !_IRENDERER2D_

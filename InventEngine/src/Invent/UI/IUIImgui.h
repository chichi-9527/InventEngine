#ifndef _IUIIMGUI_
#define _IUIIMGUI_

#include "Invent/IBase/IEventLayer.h"

struct GLFWwindow;

namespace INVENT
{
	class IUIImgui
	{
	public:
		static void Init(GLFWwindow* window);
		static void End();
		static void StartFrame();
		static void Render();

	};

	class IImguiUILayer : public IEventLayer
	{
	public:
		IImguiUILayer() = default;
		virtual ~IImguiUILayer() = default;

		virtual void RenderImgui() = 0;
	};

}

#endif // !_IUIIMGUI_


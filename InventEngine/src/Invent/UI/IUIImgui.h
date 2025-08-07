#ifndef _IUIIMGUI_
#define _IUIIMGUI_

#include "imgui.h"

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

	

}

#endif // !_IUIIMGUI_


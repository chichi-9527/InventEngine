#ifndef _IRENDERER_
#define _IRENDERER_

#include "IRendererCommand.h"

namespace INVENT
{
	class IRenderer 
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginRender();
		static void EndRender();

		static void Submit();

	private:

	};
}

#endif // !_IRENDERER_


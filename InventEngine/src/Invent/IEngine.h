#ifndef _IENGINE_
#define _IENGINE_

#include <memory>

#include "IBase/IWindow.h"

namespace INVENT
{
	// class IWindow;
	class IEngine 
	{
		friend class IWindow;
	public:
		static std::shared_ptr<IEngine> InstancePtr();

		const IWindow* GetIWindow() const { return _iwindow; }

	private:
		void SetIWindow(IWindow* window);

		IEngine();

	private:
		IWindow* _iwindow;
	};
}

#endif // !_IENGINE_

#include "IEpch.h"
#include "IEngine.h"

#include "IBase/IWindow.h"

namespace INVENT
{
	IEngine::IEngine()
		: _iwindow(nullptr)
	{
	}

	std::shared_ptr<IEngine> IEngine::InstancePtr()
	{
		static std::shared_ptr<IEngine> iengine(new IEngine);
		return iengine;
	}

	void IEngine::SetIWindow(IWindow* window)
	{
		_iwindow = window;
	}
}
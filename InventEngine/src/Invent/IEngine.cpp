#include "IEpch.h"
#include "IEngine.h"

#include "IBase/IWindow.h"

static const auto StartTimePoint = std::chrono::high_resolution_clock::now();

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

	IWindow* IEngine::GetIWindow()
	{
		return _iwindow;
	}

	float IEngine::GetEngineMilliseconds()
	{
		return std::chrono::duration<float, std::milli>(std::chrono::high_resolution_clock::now() - StartTimePoint).count();
	}

	const std::chrono::steady_clock::time_point& IEngine::GetEngineStartTimePoint()
	{
		return StartTimePoint;
	}

	void IEngine::SetIWindow(IWindow* window)
	{
		_iwindow = window;
	}
}
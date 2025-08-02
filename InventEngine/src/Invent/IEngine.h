#ifndef _IENGINE_
#define _IENGINE_

#include <memory>

#include <chrono>

namespace INVENT
{
	class IWindow;
	class IEngine 
	{
		friend class IWindow;
	public:
		static std::shared_ptr<IEngine> InstancePtr();

		IWindow* GetIWindow();

		float GetEngineMilliseconds();

		static const std::chrono::steady_clock::time_point& GetEngineStartTimePoint();

	private:
		void SetIWindow(IWindow* window);

		IEngine();

	private:
		IWindow* _iwindow;
	};
}

#endif // !_IENGINE_

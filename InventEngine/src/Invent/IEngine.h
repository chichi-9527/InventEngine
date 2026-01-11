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
		unsigned int GetWindowSizeX();
		unsigned int GetWindowSizeY();

		float GetEngineMilliseconds();

		static const std::chrono::steady_clock::time_point& GetEngineStartTimePoint();

		typedef size_t NormalInputFunctionID;
		typedef size_t CursorPositionFunctionID;
		NormalInputFunctionID RegisterNormalProcessInputFunction(std::function<void(float)>&& callback, int key);
		void CancellationNormalProcessInputFunction(NormalInputFunctionID id);
		CursorPositionFunctionID RegisterCursorPositionFunction(std::function<void(float, bool, double, double)>&& callback);
		void CancellationCursorPositionFunction(CursorPositionFunctionID id);

	private:
		void SetIWindow(IWindow* window);

		IEngine();

	private:

		std::vector<std::pair<std::function<void(float)>, int>> _normal_input_callbacks;
		std::vector<std::function<void(float, bool, double, double)>> _cursor_position_input_callbacks;

		IWindow* _iwindow;
	};
}

#endif // !_IENGINE_

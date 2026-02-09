#ifndef _IENGINE_
#define _IENGINE_

#include "ILog.h"

#include <glm/glm.hpp>

#include <memory>
#include <chrono>
#include <format>
#include <utility>

namespace INVENT
{
	class IBaseGameInstance;
	class IWindow;
	class IThreadPool;
	class IScene;

	class IEngine 
	{
		friend class IWindow;
	public:

		~IEngine();
		static std::shared_ptr<IEngine> InstancePtr();

		void Start();

		template<typename T, typename... Args>
		std::shared_ptr<T> CreateGameWindow(Args&&... args)
		{
			if constexpr (!std::is_base_of_v<IWindow, T>)
			{
				INVENT_LOG_ERROR(std::format("[IEngine] Window class type error; {}", typeid(T).name()));
				return nullptr;
			}
			 auto window = std::make_shared<T>(std::forward<Args>(args)...);
			 _iwindow_ptr = std::static_pointer_cast<IWindow>(window);
			return window;
		}
		std::shared_ptr<IWindow> GetIWindow();
		unsigned int GetWindowSizeX();
		unsigned int GetWindowSizeY();

		std::shared_ptr<IScene> GetMainScene();

		void SetGameInstance(std::shared_ptr<IBaseGameInstance> game_instance_ptr);
		std::shared_ptr<IBaseGameInstance> GetGameInstance() { return _game_instance_ptr; }

		float GetEngineMilliseconds();
		static const std::chrono::steady_clock::time_point& GetEngineStartTimePoint();

		/// <summary>
		/// default num {1,1} only set num before thread start
		/// 此线程池为可选线程池，若不调用以下任何函数则不会创建
		/// </summary>
		/// <param name="t_num">线程数量</param>
		/// <param name="p_num">优先级数量</param>
		void SetThreadPoolThreadNumAndPriorityNum(unsigned int t_num, unsigned int p_num);
		void StartThreadPool();
		void ShutdownThreadPool();

		IThreadPool* GetThreadPool() const { return _threadpool; }
		IThreadPool* GetWorkThreadPool() const { return _work_thread_pool; }

		typedef size_t NormalInputFunctionID;
		typedef size_t CursorPositionFunctionID;
		NormalInputFunctionID RegisterNormalProcessInputFunction(std::function<void(float)>&& callback, int key);
		void CancellationNormalProcessInputFunction(NormalInputFunctionID id);
		CursorPositionFunctionID RegisterCursorPositionFunction(std::function<void(float, bool, double, double)>&& callback);
		void CancellationCursorPositionFunction(CursorPositionFunctionID id);

	private:

		IEngine();

	private:

		std::vector<std::pair<std::function<void(float)>, int>> _normal_input_callbacks;
		std::vector<std::function<void(float, bool, double, double)>> _cursor_position_input_callbacks;

		std::shared_ptr<IWindow> _iwindow_ptr;
		std::shared_ptr<IBaseGameInstance> _game_instance_ptr;
		std::shared_ptr<IScene> _main_scene;

		IThreadPool* _threadpool;
		IThreadPool* _work_thread_pool;
	};
}

#endif // !_IENGINE_

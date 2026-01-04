#ifndef _IWINDOW_
#define _IWINDOW_

#include "IBaseLevel.h"
#include "IGameInstance.h"

#include "ThreadPool/IThreadPool.h"

#include "Renderer/IRenderer.h"
#include "Renderer/IRenderer2D.h"

#include <string>

struct GLFWwindow;
struct GLFWmonitor;

namespace INVENT
{
	class IWindow
	{
	public:
		IWindow(unsigned int width = 800, unsigned int height = 600, std::string title = "title");
		virtual ~IWindow();

		void Start();

		void SetWindowSize(unsigned int width, unsigned int height) const;

		unsigned int GetWidth() const { return Width; }
		unsigned int GetHeight() const { return Height; }
		float GetWindowAspect() const { return (float)Width / (float)Height; }

		IThreadPool* GetThreadPool() const { return _threadpool; }

		friend void framebuffer_size_callback(GLFWwindow* window, int width, int height);
		friend void register_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
		friend void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
		friend void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
		friend void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
		friend void cursor_enter_callback(GLFWwindow* window, int entered);

		void Close() const;

		bool IsFullScreen();
		void SetFullScreen(bool fullscreen);

		void SetLevel(IBaseLevel* level);

		std::queue<std::function<void()>>& GetMainThreadInitQueue() { return _main_thread_init_queue; }

	protected:
		// default num {1,1} only set num before thread start
		void SetThreadPoolThreadNumAndPriorityNum(unsigned int t_num, unsigned int p_num) { _threadpool->SetThreadPriorityNum(t_num, p_num); }
		void StartThreadPool() { _threadpool->Start(); }
		void ShutdownThreadPool() { _threadpool->Shutdown(); }

		virtual void Begin();

		// 异步
		// CreateLevel

		// 设置加载时的场景/关卡
		// Set

		void SetGameInstance(std::shared_ptr<IBaseGameInstance> game_instance_ptr);

		static void Render(IBaseLevel* level);

		// TEST
		virtual void Render3d(){}

	protected:
		IBaseLevel* Level;

		unsigned int Width;
		unsigned int Height;
		std::string Title;

		GLFWwindow* Window;
		GLFWmonitor* Monitor;

		// 限制帧率为与显示器帧率的反比，0 为不限制
		// 仅 Windows 下有效
		int SwapIntervalEXT = 1;
	private:
		std::shared_ptr<IBaseGameInstance> _game_instance_ptr;

		std::queue<std::function<void()>> _main_thread_init_queue;

		// 0,1: old position
		// 2,3: old size
		int _pos_size[4] = { 20,20,800,600 };

		IThreadPool* _threadpool;

		IBaseLevel* _default_level;

		float delta_time;
		std::chrono::steady_clock::time_point  last_frame;

		double cursor_xpos = 0.0, cursor_ypos = 0.0;

		bool CursorInsideWindow = false;

	private:
		void _create();
		int _glfw_init();
		void _window_size_change();
		void _process_input(float delta);

	};
}



#endif // !_IWINDOW_
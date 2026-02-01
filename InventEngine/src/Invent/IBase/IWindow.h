#ifndef _IWINDOW_
#define _IWINDOW_

#include "Renderer/IRenderer.h"
#include "Renderer/IRenderer2D.h"

#include <string>

struct GLFWwindow;
struct GLFWmonitor;

namespace INVENT
{
	class IRenderThread;
	class IBaseLevel;

	class IWindow
	{
		friend void framebuffer_size_callback(GLFWwindow* window, int width, int height);
		friend void register_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
		friend void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
		friend void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
		friend void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
		friend void cursor_enter_callback(GLFWwindow* window, int entered);
	public:
		IWindow(unsigned int width = 800, unsigned int height = 600, std::string title = "title");
		virtual ~IWindow();

		bool Start();

		void SetWindowSize(unsigned int width, unsigned int height) const;

		unsigned int GetWidth() const { return Width; }
		unsigned int GetHeight() const { return Height; }
		float GetWindowAspect() const { return (float)Width / (float)Height; }

		GLFWwindow* GetGLFWWindow() const;

		void Close() const;

		bool IsFullScreen();
		void SetFullScreen(bool fullscreen);

		void SetLevel(IBaseLevel* level);

		std::queue<std::function<void()>>& GetMainThreadInitQueue() { return _main_thread_init_queue; }

	protected:
		virtual void Begin();

		static void Render(IBaseLevel* level);

		// TEST
		virtual void Render3d(){}

	private:
		void _create_window();
		int _glfw_init();
		void _window_size_change();

		// 已废弃 现在通过注册回调函数
		//void _process_input(float delta);

		void _process_input_callback(float delta);

	protected:
		IBaseLevel* Level;

		unsigned int Width;
		unsigned int Height;
		std::string Title;

		

	private:
		std::shared_ptr<IRenderThread> _render_thread;

		std::queue<std::function<void()>> _main_thread_init_queue;

		// 0,1: old position
		// 2,3: old size
		int _pos_size[4] = { 20,20,800,600 };

		IBaseLevel* _default_level;

		float delta_time;
		std::chrono::steady_clock::time_point  last_frame;

		double cursor_xpos = 0.0, cursor_ypos = 0.0;

		bool CursorInsideWindow = false;

	};
}



#endif // !_IWINDOW_
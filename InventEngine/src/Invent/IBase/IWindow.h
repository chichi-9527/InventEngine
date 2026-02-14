#ifndef _IWINDOW_
#define _IWINDOW_

#include <string>

struct GLFWwindow;
struct GLFWmonitor;

namespace INVENT
{
	class IRenderThread;
	class IBaseLevel;

	class IWindow
	{
		friend class IEngine;
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

	protected:
		virtual void Begin();
		virtual void End();
		// TEST
		virtual void Render3d(){}

	private:
		void _create_window();
		int _glfw_init();
		void _window_size_change();

		void _process_input_callback(float delta);

		void _framebuffer_size_callback(int width, int height);
		void _register_key_callback(int key, int scancode, int action, int mods);
		void _cursor_position_callback(double xpos, double ypos);
		void _mouse_button_callback(int button, int action, int mods);
		void _scroll_callback(double xoffset, double yoffset);
		void _cursor_enter_callback(int entered);

	protected:
		IBaseLevel* Level;

		unsigned int Width;
		unsigned int Height;
		std::string Title;

		

	private:
		std::shared_ptr<IRenderThread> _render_thread;

		// 0,1: old position
		// 2,3: old size
		int _pos_size[4] = { 20,20,800,600 };

		float delta_time;
		std::chrono::steady_clock::time_point  last_frame;

		double cursor_xpos = 0.0, cursor_ypos = 0.0;

		bool CursorInsideWindow = false;

	};
}



#endif // !_IWINDOW_
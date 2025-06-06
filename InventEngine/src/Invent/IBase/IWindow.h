#ifndef _IWINDOW_
#define _IWINDOW_

#include "IBaseLevel.h"
#include "IGameInstance.h"

#include <string>
#include <GLFW/glfw3.h>

namespace INVENT
{
	class IWindow
	{
	public:
		IWindow(unsigned int width = 800, unsigned int height = 600, std::string title = "title");
		virtual ~IWindow();

		void Start();

		friend void framebuffer_size_callback(GLFWwindow* window, int width, int height);
		friend void register_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

	protected:
		void Close();

		void SetLevel(IBaseLevel* level);

		void SetGameInstance(std::shared_ptr<IBaseGameInstance> game_instance_ptr);

	protected:
		IBaseLevel* Level;

		unsigned int Width;
		unsigned int Height;
		std::string Title;

		GLFWwindow* Window;
	private:
		std::shared_ptr<IBaseGameInstance> _game_instance_ptr;

		IBaseLevel* _default_level;

		float delta_time;
		float last_frame;

	private:
		void _create();
		int _glfw_init();
		void _window_size_change();
		void _process_input(float delta);

	};
}



#endif // !_IWINDOW_
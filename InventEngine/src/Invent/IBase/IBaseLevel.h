#ifndef _IBASELEVEL_
#define _IBASELEVEL_

#include "IEventLayer.h"

#include <vector>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace INVENT
{
	class IBaseLevel : public IEventLayer
	{
	public:
		friend class IWindow;
		friend void register_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

		IBaseLevel();
		virtual ~IBaseLevel();

		virtual void Update(float delta);

	protected:
		void SetClearColor(float red, float green, float blue, float alpha);
		void SetClearColor(glm::vec4 color);


		void AddLayer(IEventLayer* layer);
		void PopLayer();
		void PopLayer(IEventLayer* layer);

		IEventLayer* CreateLayer();
		void EraseLayer(IEventLayer* layer);

		float GetAspectRatio() { return _window_size.x / _window_size.y; }


	private:
		// clear opengl buffer
		void _clear();

		void _clear_color();

	protected:
		IEventLayer* ObjectEventLayer;
		IEventLayer* UIEventLayer;
	private:
		std::vector<IEventLayer*> _event_layers;

		std::vector<IEventLayer*> layers;

		glm::vec2 _window_size;

		glm::vec4 _clear_color_vec;


	};

}


#endif // !_IBASELEVEL_

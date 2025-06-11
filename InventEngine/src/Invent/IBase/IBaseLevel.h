#ifndef _IBASELEVEL_
#define _IBASELEVEL_

#include "IEventLayer.h"

#include "IBaseActor.h"
#include "2D/ISquare2dActor.h"

#include "ILog.h"

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

		// must Inherited from IEventLayer
		template<typename T>
		T* CreateLayer()
		{
			if (!std::is_base_of_v<IEventLayer, T>)
			{
				INVENT_ASSERT(false, "this class is not Inherited from IEventLayer");
				INVENT_ASSERT(false, std::string(typeid(T).name()));
				return nullptr;
			}
			T* layer = new T;
			layers.push_back((IEventLayer*)layer);
			return layer;
		}

		void EraseLayer(IEventLayer* layer);

		float GetAspectRatio() const { return _window_size.x / _window_size.y; }

		// must Inherited from IBaseActor
		// Instances will render if they inherit from the following base classes:
		// <ISquare2dActor> 
		template<typename T>
		T* CreateActor()
		{
			if (!std::is_base_of_v<IBaseActor, T>)
			{
				INVENT_ASSERT(false, "this class is not Inherited from IBaseActor");
				INVENT_ASSERT(false, std::string(typeid(T).name()));
				return nullptr;
			}

			T* actor = new T;
			_actors.push_back((IBaseActor*)actor);

			if (!std::is_base_of_v<ISquare2dActor, T>)
				_square_2d_actors.push_back((ISquare2dActor*)actor);

			return actor;
		}

		void DeleteActor(const IBaseActor* actor);

		// 添加到此的实例将在关卡析构时释放，谨慎使用
		void AddActor(IBaseActor* actor);
		// will render ,不会自动释放，谨慎使用
		void AddSquare2dActor(ISquare2dActor* actor);
		void EraseSquare2dActor(ISquare2dActor* actor);

	private:
		// clear opengl buffer
		void _clear();

		void _clear_color();

	protected:
		IEventLayer* ObjectEventLayer;

	private:
		// 事件分发顺序
		std::vector<IEventLayer*> _event_layers;
		// layer 管理
		std::vector<IEventLayer*> layers;
		// 所有 继承自IBaseActor 的实例，即在关卡中 Update 的实例
		// 一般情况下使用 CreateActor 创建
		// 凡添加到本数组的实例统一由 this Level 管理，不需要手动释放内存
		std::vector<IBaseActor*> _actors;
		// 以下实例数组为记录可渲染实例，不需要释放
		std::vector<ISquare2dActor*> _square_2d_actors;

		glm::vec2 _window_size;

		glm::vec4 _clear_color_vec;


	};

}


#endif // !_IBASELEVEL_

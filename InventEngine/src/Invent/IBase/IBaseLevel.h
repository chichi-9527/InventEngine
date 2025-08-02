#ifndef _IBASELEVEL_
#define _IBASELEVEL_

#include "IEventLayer.h"

#include "IBaseActor.h"
#include "IController.h"
#include "ICamera.h"

#include "2D/ISquare2dActor.h"
#include "2D/ITileMap.h"

#include "IPhysicsCollision/ICollisionHandling.h"

#include "ThreadPool/IThreadPool.h"

#include "ILog.h"

#include <vector>
#include <mutex>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace INVENT
{
	class IBaseActor;
	class IBaseLevel : public IEventLayer
	{
		friend class ICollisionHandling;
	public:
		friend class IWindow;
		friend void register_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

		IBaseLevel();
		virtual ~IBaseLevel();

		virtual void Update(float delta);

		// need Inherited from IPlayerControllerBase
		// and auto SetController if inherited
		template<typename T, typename ...Args>
		std::shared_ptr<T> CreateControllerPtr(Args&&... args)
		{
			std::shared_ptr<T> controller;
			if (std::is_base_of_v<IPlayerControllerBase, T>)
			{
				controller = std::make_shared<T>(std::forward<Args>(args)...);
				SetController(controller);
			}
			return controller;
		}

		void SetController(std::shared_ptr<IPlayerControllerBase> controller);
		std::shared_ptr<IPlayerControllerBase> GetController() { return _controller_ptr; }

		template<typename T>
		std::shared_ptr<T> GetController() 
		{ 
			return std::static_pointer_cast<T>(_controller_ptr);
		}

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
		// Instances will find Collision and Physics Components if they inherit from the following base classes:
		// <IActor2D> 
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

			GetIWindowThreadPool()->Submit(0, [this, actor]() {
				AddActor((IBaseActor*)actor);

				this->AddEventObj((IBaseEventFunction*)(actor));

				if (std::is_base_of_v<ISquare2dActor, T>)
					AddSquare2dActor((ISquare2dActor*)actor);
				else if (std::is_base_of_v<ITileMap, T>)
					AddTileMap((ITileMap*)actor);

				});

			/*if (std::is_base_of_v<ISquare2dActor, T>)
				_square_2d_actors.push_back((ISquare2dActor*)actor);*/
			// else if()

			return actor;
		}

		void DeleteActor(const IBaseActor* actor);

		// 添加到此的实例将在关卡析构时释放，谨慎使用
		void AddActor(IBaseActor* actor);
		// will render ,不会自动释放，谨慎使用
		void AddSquare2dActor(ISquare2dActor* actor);
		void EraseSquare2dActor(ISquare2dActor* actor);

		void AddTileMap(ITileMap* actor);
		void EraseTileMap(ITileMap* actor);

	public:
		void AddStaticCollider(IColliderBase* collider);
		void EraseStaticCollider(const IColliderBase* collider);
		void AddStaticColliders(const std::vector<IColliderBase*>& collider);
		void AddDynamicCollider(IColliderBase* collider);
		void EraseDynamicCollider(const IColliderBase* collider);
		void AddDynamicColliders(const std::vector<IColliderBase*>& collider);

		IThreadPool* GetIWindowThreadPool();

	private:
		// clear opengl buffer
		void _clear();

		void _clear_color() const;

		void _collision_detection();
		void _deal_collision();

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
		std::vector<ITileMap*>		 _tile_map_actors;

		std::vector<IColliderBase*> _static_colliders;
		std::vector<IColliderBase*> _dynamic_colliders;
		// 碰撞体回调函数，统一在主线程调用
		// 更改代码时，注意多线程资源竞争
		std::vector<std::function<void()>> _collider_callbacks;
		std::vector<std::function<void()>> _collision_handings;
		// 由 ICollisionHandling 管理，若更改逻辑，需要更改 ICollisionHandling::StartCollisionHandle 函数中逻辑
		bool _is_over_collision_detection = true;

		std::shared_ptr<IPlayerControllerBase> _controller_ptr;

		glm::vec2 _window_size;

		glm::vec4 _clear_color_vec;

		std::mutex _mutex;
		std::mutex _tile_map_mutex;
		std::mutex _collision_mutex;
		std::mutex _colliders_mutex;

		ICollisionHandling* _colli_handler;

	};

}


#endif // !_IBASELEVEL_

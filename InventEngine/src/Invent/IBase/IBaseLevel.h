#ifndef _IBASELEVEL_
#define _IBASELEVEL_

//#include "IController.h"
//#include "ICamera.h"

#include "Invent/UI/IUIImgui.h"

#include "Invent/ThreadPool/IThreadPool.h"
#include "IEventLayer.h"
#include "Invent/ILog.h"

#include <glm/glm.hpp>

#include <format>
#include <vector>
#include <mutex>

struct GLFWwindow;

namespace INVENT
{
	class IActor;
	class ISquare2dActor;
	class ITileMap;
	class IColliderBase;

	class IBaseLevel : public IEventLayer
	{
		friend class IScene;
	public:
		
		IBaseLevel(const glm::vec3& position = {});
		virtual ~IBaseLevel();

		virtual void Begin() = 0;
		virtual void Update(float delta) = 0;
		virtual void End() = 0;

		/// <summary>
		/// 创建并返回类型为 T 的 actor 实例
		/// actor 将在关卡失效后失效，生命周期有关卡管理
		/// </summary>
		/// <typeparam name="T">要创建的 actor 类型。T 必须继承自 IActor；</typeparam>
		/// <returns>指向新分配的 T 实例的指针；如果类型不满足要求则返回 nullptr。</returns>
		template<typename T, typename ... Args>
		T* CreateActor(Args&&... args)
		{
			if constexpr (!std::is_base_of_v<IActor, T>)
			{
				INVENT_LOG_ERROR(std::format("[BaseLevel] Actor class type error; {}", typeid(T).name()));
				return nullptr;
			}

			T* actor = new T(std::forward<Args>(args)...);

			_get_engine_work_thread_pool()->Submit(0, [this, actor]() {

				_add2_all_actors(static_cast<IActor*>(actor));

				if constexpr (std::is_base_of_v<ISquare2dActor, T>)
				{
					_add_actor<ISquare2dActor>(static_cast<ISquare2dActor*>(actor));
				}
				if constexpr (std::is_base_of_v<ITileMap, T>)
				{
					_add_actor<ITileMap>(static_cast<ITileMap*>(actor));
				}

				});

			return actor;
		}

		/// <summary>
		/// 根据模板类型将指定的 actor 从各类管理集合中移除。
		/// </summary>
		/// <typeparam name="ATy">actor 的静态类型。编译期将根据此类型来选择相应的移除行为。</typeparam>
		/// <param name="actor">指向要移除的 actor 的指针的引用。</param>
		template<typename ATy>
		void DestoryActor(ATy*& actor)
		{
			if constexpr (!std::is_base_of_v<IActor, ATy>)
			{
				return;
			}

			_destory_actor_funcs.push_back([this, actor]() {
				_erase_from_all_actors(static_cast<IActor*>(actor));
				});

			if constexpr (std::is_base_of_v<ISquare2dActor, ATy>)
			{
				_erase_actor<ISquare2dActor>(static_cast<ISquare2dActor*>(actor));
			}
			if constexpr (std::is_base_of_v<ITileMap, ATy>)
			{
				_erase_actor<ITileMap>(static_cast<ITileMap*>(actor));
			}

		}

		/// <summary>
		/// 设置此关卡位置，在场景中渲染时将参考此位置
		/// 设置位置时将异步更新所有 actor 的位置
		/// </summary>
		/// <param name="position"></param>
		void SetPosition(const glm::vec3& position);
		const glm::vec3& GetPosition() const { return _position; }

		// 碰撞体
		void AddStaticCollider(IColliderBase* collider);
		void EraseStaticCollider(const IColliderBase* collider);
		void AddStaticColliders(const std::vector<IColliderBase*>& collider);
		void AddDynamicCollider(IColliderBase* collider);
		void EraseDynamicCollider(const IColliderBase* collider);
		void AddDynamicColliders(const std::vector<IColliderBase*>& collider);

	private:
		void _add2_all_actors(IActor* actor_ptr);
		void _erase_from_all_actors(IActor*& actor_ptr);

		template<typename ActorT>
		void _add_actor(ActorT* actor_t)
		{
			std::lock_guard<std::mutex> lock(_all_actors_mutex);
			std::get<std::vector<ActorT*>>(_actor_vectors).emplace_back(actor_t);
		}

		template<typename ActorT>
		void _erase_actor(ActorT* actor_t)
		{
			std::lock_guard<std::mutex> lock(_all_actors_mutex);
			auto& arr = std::get<std::vector<ActorT*>>(_actor_vectors);
			for (auto& actor : arr)
			{
				if (actor == actor_t)
				{
					std::swap(actor, arr.back());
					arr.pop_back();
				}
			}
		}

		IThreadPool* _get_engine_work_thread_pool();

		void _recalculate_actors_position();
		void _update_actor_position(IActor* actor);

	private:
		std::tuple<std::vector<ISquare2dActor*>
			, std::vector<ITileMap*>> _actor_vectors;

		std::vector<IActor*> _all_actors;
		std::vector<std::function<void()>> _destory_actor_funcs;
		std::vector<IColliderBase*> _static_colliders;
		std::vector<IColliderBase*> _dynamic_colliders;

		std::mutex _all_actors_mutex;
		std::mutex _colliders_mutex;

		glm::vec3 _position;


	/// <summary>
	/// ///////////////////////////////////// old 
	/// </summary>

	protected:

		float GetAspectRatio() const { return _window_size.x / _window_size.y; }

	private:
		//std::shared_ptr<IPlayerControllerBase> _controller_ptr;

		glm::vec2 _window_size;
		

		

		IImguiUILayer* _imgui_showing_layer = nullptr;

	};

}


#endif // !_IBASELEVEL_

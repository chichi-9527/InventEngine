#ifndef _ISCENE_
#define _ISCENE_

#include "Invent/ThreadPool/IThreadPool.h"
#include "IBaseLevel.h"
#include "Invent/ITools/ISafeFastPtrVector.h"
#include "Invent/ILog.h"

#include <glm/glm.hpp>

#include <memory>
#include <vector>
#include <format>

namespace INVENT
{
	class ILevel;
	class IPlayerControllerBase;

	class IScene : public IBaseLevel
	{
		friend class ICollisionHandling;
		friend class IWindow;
		IScene();
	public:
		struct ShowLevelActorVecters 
		{
			std::vector<ISquare2dActor*> Square2dActors;
			std::vector<ITileMap*> TileMaps;

			void Clear()
			{
				Square2dActors.clear();
				TileMaps.clear();
			}
		};
		
		~IScene();

		virtual void Begin() override;
		virtual void Update(float delta) override;
		virtual void End() override;

		/// <summary>
		/// 创建 PlayerController
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <typeparam name="...Args"></typeparam>
		/// <param name="...args"></param>
		/// <returns>如果不是 IPlayerControllerBase 子类将返回 nullptr</returns>
		template<typename T, typename ...Args>
		std::weak_ptr<T> CreateController(Args&&... args)
		{
			if constexpr (!std::is_base_of_v<IPlayerControllerBase, T>)
			{
				INVENT_LOG_ERROR(std::format("[Scene] Controller class type error; {}", typeid(T).name()));
				return nullptr;
			}
			std::shared_ptr<T> controller = std::make_shared<T>(std::forward<Args>(args)...);
			_set_controller(std::static_pointer_cast<IPlayerControllerBase>(controller));
			return controller;
		}

		std::weak_ptr<IPlayerControllerBase> GetController() { return _controller_ptr; }
		template<typename T>
		std::weak_ptr<T> GetController()
		{
			return std::static_pointer_cast<T>(_controller_ptr);
		}

		void DestoryController();

		typedef size_t ILevelID;
		/// <summary>
		/// 创建关卡实例
		/// </summary>
		/// <typeparam name="T">关卡类</typeparam>
		/// <returns>关卡id, 若不是 ILevel 的派生类将不创建实例并返回错误值 size_t(-1)</returns>
		template<typename T>
		ILevelID CreateLevelInstance(const glm::vec3& position)
		{
			if constexpr (!std::is_base_of_v<ILevel, T>)
			{
				INVENT_LOG_ERROR(std::format("[Scene] Level class type error; {}", typeid(T).name()));
				return size_t(-1);
			}

			auto id = _all_levels.size();
			if (_id_queue.empty())
			{
				_all_levels.push_back(nullptr);
			}
			else
			{
				std::lock_guard<std::mutex> lock(_all_levels_mutex);
				id = _id_queue.front();
				_id_queue.pop();
			}

			_create_pool.Submit(0, [this, id, &position]() {
				
				auto level = new T(position);

				std::lock_guard<std::mutex> lock(_all_levels_mutex);

				_all_levels[id] = (ILevel*)level;
				});
			
			return id;
		}

		/// <summary>
		/// 先创建关卡实例，在将此实例加入显示列表
		/// </summary>
		/// <typeparam name="T">关卡类</typeparam>
		/// <param name="position">关卡实例在场景中的位置</param>
		/// <returns>关卡id</returns>
		template<typename T>
		ILevelID ShowLevelInstance(const glm::vec3& position = {})
		{
			auto id = CreateLevelInstance<T>(position);
			ShowLevelInstance(id);

			return id;
		}

		void ShowLevelInstance(ILevelID id);

		/// <summary>
		/// 隐藏指定的关卡实例。不会销毁实例
		/// </summary>
		/// <param name="id">要隐藏的关卡实例的标识</param>
		void HideLevelInstance(ILevelID id);
		/// <summary>
		/// 销毁关卡实例 
		/// 销毁后使用 id 获取关卡实例时若未复用将返回 nullptr，若已复用将返回新的关卡实例（可能会返回与预期不同的实例）
		/// </summary>
		/// <param name="id">要销毁的关卡实例的标识</param>
		void DestoryLevelInstance(ILevelID id);

		ShowLevelActorVecters& GetShowLevelActorVectors();
		
		static std::shared_ptr<IScene> CreateInstancePtr();

	protected:

		virtual IsEventDone EVENT_CURSOR_POSITION_FRAME(float delta, bool cursor_inside_window, double xpos, double ypos) override;
		virtual IsEventDone EVENT_KEY_FRAME(float delta, GLFWwindow* glfw_window) override;
		virtual IsEventDone EVENT_KEY(int key, int action, int mods) override;
		virtual IsEventDone EVENT_MOUSE_BUTTON(double xpos, double ypos, int button, int action, int mods) override;
		virtual IsEventDone EVNET_SCROLL(double cursor_xpos, double cursor_ypos, double xoffset, double yoffset) override;
		virtual IsEventDone EVENT_CURSOR_POSITION(double xpos, double ypos) override;

	private:
		void _set_controller(std::shared_ptr<IPlayerControllerBase> controller_ptr);

		void _collision_detection();
		void _deal_collision();

		void _add_layer(IEventLayer* layer);
		void _pop_layer();
		void _pop_layer(IEventLayer* layer);

	private:
		IThreadPool _create_pool;
		std::shared_ptr<IPlayerControllerBase> _controller_ptr;
		std::vector<ILevel*> _all_levels;
		std::queue<size_t> _id_queue;
		std::unordered_set<ILevelID> _show_levels;
		std::vector<IColliderBase*> _all_show_static_colliders;
		std::vector<IColliderBase*> _all_show_dynamic_colliders;

		std::mutex _all_levels_mutex;
		std::mutex _show_levels_mutex;

		// 碰撞体回调函数，统一在主线程调用
		// 更改代码时，注意多线程资源竞争
		std::vector<std::function<void()>> _collider_callbacks;
		std::vector<std::function<void()>> _collision_handings;
		std::mutex _collision_mutex;
		ICollisionHandling* _colli_handler;

		ShowLevelActorVecters _show_level_actor_vectors;
		std::vector<IEventLayer*> _event_layers;

		// 由 ICollisionHandling 管理，若更改逻辑，需要更改 ICollisionHandling::StartCollisionHandle 函数中逻辑
		bool _is_over_collision_detection = true;

	};


}
#endif // !_ISCENE_


#include "IEpch.h"
#include "IScene.h"

#include "IEngine.h"
#include "ILevel.h"
#
#include "IPhysicsCollision/ICollisionHandling.h"

namespace INVENT
{
	
	IScene::IScene()
		: IBaseLevel()
		, IEventLayer()
	{
		_colli_handler = new ICollisionHandling(this);
		_all_show_static_colliders.reserve((size_t)1 << 10);
		_all_show_dynamic_colliders.reserve((size_t)1 << 10);
	}

	IScene::~IScene()
	{
		for (auto& level : _all_levels)
		{
			if (level)
			{
				delete level;
			}
		}

		if (_colli_handler)
		{
			delete _colli_handler;
			_colli_handler = nullptr;
		}
	}

	void IScene::Begin()
	{
		_create_pool.Start();
	}

	void IScene::Update(float delta)
	{
		IBaseLevel::Update(delta);
		for (const auto level_id : _show_levels)
		{
			if (auto level = _all_levels[level_id])
			{
				level->Update(delta);
				// 碰撞检查完成后再统一销毁 Actor
				if (_is_over_collision_detection)
				{
					for (auto& func : level->_destory_actor_funcs)
					{
						if (func) func();
					}
					level->_destory_actor_funcs.clear();
				}
			}
		}

		// 碰撞检测
		_deal_collision();
		if (_is_over_collision_detection)
		{
			for (auto& func : _destory_actor_funcs)
			{
				if (func) func();
			}
			_destory_actor_funcs.clear();
			_collision_detection();
		}
	}

	void IScene::End()
	{
		_create_pool.Shutdown();
	}

	void IScene::DestoryController()
	{
		_controller_ptr = nullptr;
	}

	void IScene::ShowLevelInstance(ILevelID id)
	{
		std::lock_guard<std::mutex> lock(_show_levels_mutex);
		_show_levels.insert(id);
	}

	void IScene::HideLevelInstance(ILevelID id)
	{
		std::lock_guard<std::mutex> lock(_show_levels_mutex);
		_show_levels.erase(id);
	}

	void IScene::DestoryLevelInstance(ILevelID id)
	{
		HideLevelInstance(id);
		if (id < _all_levels.size())
		{
			if (_all_levels[id])
			{
				delete _all_levels[id];
				_all_levels[id] = nullptr;
				_id_queue.push(id);
			}
		}
	}

	std::shared_ptr<IScene> IScene::CreateInstancePtr()
	{
		auto scene = std::make_shared<IScene>();
		return scene;
	}

	void IScene::_set_controller(std::shared_ptr<IPlayerControllerBase>& controller_ptr)
	{
		_controller_ptr = controller_ptr;
	}

	void IScene::_collision_detection()
	{
		for (const auto level_id : _show_levels)
		{
			if (auto level = _all_levels[level_id])
			{
				_all_show_static_colliders.insert(_all_show_static_colliders.end(), level->_static_colliders.begin(), level->_static_colliders.end());
				_all_show_dynamic_colliders.insert(_all_show_dynamic_colliders.end(), level->_dynamic_colliders.begin(), level->_dynamic_colliders.end());
			}
		}
		_all_show_static_colliders.insert(_all_show_static_colliders.end(), _static_colliders.begin(), _static_colliders.end());
		_all_show_dynamic_colliders.insert(_all_show_dynamic_colliders.end(), _dynamic_colliders.begin(), _dynamic_colliders.end());
		_colli_handler->StartCollisionHandleDynamic(_all_show_static_colliders, _all_show_dynamic_colliders);
	}

	void IScene::_deal_collision()
	{
		{
			std::lock_guard<std::mutex> lock(_collision_mutex);
			for (auto& func : _collision_handings)
			{
				func();
			}
			_collision_handings.clear();
		}
		{
			std::lock_guard<std::mutex> lock(_collision_mutex);
			for (auto& func : _collider_callbacks)
			{
				func();
			}
			_collider_callbacks.clear();
		}

	}

	
	
}

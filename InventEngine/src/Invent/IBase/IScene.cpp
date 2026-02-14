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
	{
		_colli_handler = new ICollisionHandling(this);
		_all_show_static_colliders.reserve((size_t)1 << 10);
		_all_show_dynamic_colliders.reserve((size_t)1 << 10);

		_event_layers.reserve((size_t)1 << 4);
		_event_layers.push_back(this);
		// controller layer
		_event_layers.push_back(nullptr);
	}

	IScene::~IScene()
	{
		
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
			if (_all_levels.empty())
			{
				break;
			}
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

		if (_colli_handler)
		{
			_colli_handler->Shutdown();
			delete _colli_handler;
			_colli_handler = nullptr;
		}

		for (auto& level : _all_levels)
		{
			if (level)
			{
				level->End();
				delete level;
			}
		}

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
				_all_levels[id]->End();
				delete _all_levels[id];
				_all_levels[id] = nullptr;
				_id_queue.push(id);
			}
		}
	}

	IScene::ShowLevelActorVecters& IScene::GetShowLevelActorVectors()
	{
		_show_level_actor_vectors.Clear();

		for (const auto level_id : _show_levels)
		{
			if (_all_levels.empty())
			{
				break;
			}
			if (auto level = _all_levels[level_id])
			{
				auto& [suqare_actors, tilemap_actors] = level->_actor_vectors;

				_show_level_actor_vectors.Square2dActors.insert(_show_level_actor_vectors.Square2dActors.end(),
					suqare_actors.begin(),
					suqare_actors.end());

				_show_level_actor_vectors.TileMaps.insert(_show_level_actor_vectors.TileMaps.end(),
					tilemap_actors.begin(),
					tilemap_actors.end());

			}
		}

		auto& [suqare_actors, tilemap_actors] = this->_actor_vectors;

		_show_level_actor_vectors.Square2dActors.insert(_show_level_actor_vectors.Square2dActors.end(),
			suqare_actors.begin(),
			suqare_actors.end());

		_show_level_actor_vectors.TileMaps.insert(_show_level_actor_vectors.TileMaps.end(),
			tilemap_actors.begin(),
			tilemap_actors.end());


		return _show_level_actor_vectors;
	}

	std::shared_ptr<IScene> IScene::CreateInstancePtr()
	{
		auto scene = std::shared_ptr<IScene>(new IScene);
		return scene;
	}

	IEventLayer::IsEventDone IScene::EVENT_CURSOR_POSITION_FRAME(float delta, bool cursor_inside_window, double xpos, double ypos)
	{
		for (const auto level_id : _show_levels)
		{
			if (_all_levels.empty())
			{
				break;
			}
			if (auto level = _all_levels[level_id])
			{
				level->EVENT_CURSOR_POSITION_FRAME(delta, cursor_inside_window, xpos, ypos);
			}
		}
		return IEventLayer::EVENT_CURSOR_POSITION_FRAME(delta, cursor_inside_window, xpos, ypos);
	}

	IEventLayer::IsEventDone IScene::EVENT_KEY_FRAME(float delta, GLFWwindow* glfw_window)
	{
		for (const auto level_id : _show_levels)
		{
			if (_all_levels.empty())
			{
				break;
			}
			if (auto level = _all_levels[level_id])
			{
				level->EVENT_KEY_FRAME(delta, glfw_window);
			}
		}
		return IEventLayer::EVENT_KEY_FRAME(delta, glfw_window);
	}

	IEventLayer::IsEventDone IScene::EVENT_KEY(int key, int action, int mods)
	{
		for (const auto level_id : _show_levels)
		{
			if (_all_levels.empty())
			{
				break;
			}
			if (auto level = _all_levels[level_id])
			{
				level->EVENT_KEY(key, action, mods);
			}
		}
		return IEventLayer::EVENT_KEY(key, action, mods);
	}

	IEventLayer::IsEventDone IScene::EVENT_MOUSE_BUTTON(double xpos, double ypos, int button, int action, int mods)
	{
		for (const auto level_id : _show_levels)
		{
			if (_all_levels.empty())
			{
				break;
			}
			if (auto level = _all_levels[level_id])
			{
				level->EVENT_MOUSE_BUTTON(xpos, ypos, button, action, mods);
			}
		}
		return IEventLayer::EVENT_MOUSE_BUTTON(xpos, ypos, button, action, mods);
	}

	IEventLayer::IsEventDone IScene::EVNET_SCROLL(double cursor_xpos, double cursor_ypos, double xoffset, double yoffset)
	{
		for (const auto level_id : _show_levels)
		{
			if (_all_levels.empty())
			{
				break;
			}
			if (auto level = _all_levels[level_id])
			{
				level->EVNET_SCROLL(cursor_xpos, cursor_ypos, xoffset, yoffset);
			}
		}
		return IEventLayer::EVNET_SCROLL(cursor_xpos, cursor_ypos, xoffset, yoffset);
	}

	IEventLayer::IsEventDone IScene::EVENT_CURSOR_POSITION(double xpos, double ypos)
	{
		for (const auto level_id : _show_levels)
		{
			if (_all_levels.empty())
			{
				break;
			}
			if (auto level = _all_levels[level_id])
			{
				level->EVENT_CURSOR_POSITION(xpos, ypos);
			}
		}
		return IEventLayer::EVENT_CURSOR_POSITION(xpos, ypos);
	}

	void IScene::_set_controller(std::shared_ptr<IPlayerControllerBase> controller_ptr)
	{
		_controller_ptr = controller_ptr;
		_event_layers[1] = (IEventLayer*)controller_ptr.get();
	}

	void IScene::_collision_detection()
	{
		for (const auto level_id : _show_levels)
		{
			if (_all_levels.empty())
			{
				break;
			}
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
			std::lock_guard<std::mutex> lock(_handing_mutex);
			for (auto& func : _collision_handings)
			{
				if (func)
				{
					func();
				}
				
			}
			_collision_handings.clear();
		}
		{
			std::lock_guard<std::mutex> lock(_callback_mutex);
			for (auto& func : _collider_callbacks)
			{
				func();
			}
			_collider_callbacks.clear();
		}

	}

	void IScene::_add_layer(IEventLayer* layer)
	{
		_event_layers.push_back(layer);
	}

	void IScene::_pop_layer()
	{
		_event_layers.pop_back();
	}

	void IScene::_pop_layer(IEventLayer* layer)
	{
		for (auto iter = _event_layers.begin(); iter != _event_layers.end(); iter++)
		{
			if ((*iter) == layer)
			{
				_event_layers.erase(iter);
				break;
			}
		}
	}

	
	
}

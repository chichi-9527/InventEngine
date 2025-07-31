#include "IEpch.h"
#include "IBaseLevel.h"
#include "IBaseActor.h"

#include "IEngine.h"

namespace INVENT
{

	IBaseLevel::IBaseLevel()
		: ObjectEventLayer(nullptr)
		, _window_size({ 0.0f, 0.0f })
		, _clear_color_vec({ 0.2f,0.2f,0.2f,1.0f })
	{
		ObjectEventLayer = CreateLayer<IEventLayer>();
		this->SetAllEventReturn(false);

		AddLayer(ObjectEventLayer);
		AddLayer(this);

		_colli_handler = new ICollisionHandling(this);
	}

	IBaseLevel::~IBaseLevel()
	{
		for (auto layer : layers)
		{
			if (layer)
			{
				delete layer;
				layer = nullptr;
			}
		}

		for (auto actor : _actors)
		{
			if (actor)
			{
				delete actor;
				actor = nullptr;
			}
		}

		if (_colli_handler)
		{
			delete _colli_handler;
			_colli_handler = nullptr;
		}
	}

	void IBaseLevel::Update(float delta)
	{
		for (auto actor : _actors)
		{
			if (actor) actor->Update(delta);
		}

		// 碰撞检测
		_deal_collision();
	}

	void IBaseLevel::SetController(std::shared_ptr<IPlayerControllerBase> controller)
	{
		if (_controller_ptr)
		{
			this->EraseLayer(_controller_ptr.get());
		}
		_controller_ptr = controller;
		if (_controller_ptr)
		{
			this->AddLayer(_controller_ptr.get());
		}
	}

	void IBaseLevel::SetClearColor(float red, float green, float blue, float alpha)
	{
		_clear_color_vec.r = red;
		_clear_color_vec.g = green;
		_clear_color_vec.b = blue;
		_clear_color_vec.a = alpha;
	}

	void IBaseLevel::SetClearColor(glm::vec4 color)
	{
		_clear_color_vec = color;
	}

	void IBaseLevel::DeleteActor(const IBaseActor* actor)
	{
		// 删除 _actors 中的元素
		auto iter = std::find(_actors.begin(), _actors.end(), actor);
		if (iter != _actors.end())
		{
			*iter = _actors.back();
			_actors.pop_back();
		}

		// 删除 _square_2d_actors 中的元素
		EraseSquare2dActor((ISquare2dActor*)actor);
		// 删除 _tile_map_actors 中的元素
		EraseTileMap((ITileMap*)actor);

		// 删除其他记录实例类中的元素


		// delete actor at last
		delete actor;
	}

	void IBaseLevel::AddActor(IBaseActor* actor)
	{
		actor->SetLevel(this);
		std::lock_guard<std::mutex> lock(_mutex);
		_actors.push_back(actor);
	}

	void IBaseLevel::AddSquare2dActor(ISquare2dActor * actor)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_square_2d_actors.push_back(actor);
	}

	void IBaseLevel::EraseSquare2dActor(ISquare2dActor* actor)
	{
		auto iter = std::find(_square_2d_actors.begin(), _square_2d_actors.end(), actor);
		if (iter != _square_2d_actors.end())
		{
			std::lock_guard<std::mutex> lock(_mutex);
			*iter = _square_2d_actors.back();
			_square_2d_actors.pop_back();
		}
	}

	void IBaseLevel::AddTileMap(ITileMap* actor)
	{
		std::lock_guard<std::mutex> lock(_tile_map_mutex);
		_tile_map_actors.push_back(actor);
	}

	void IBaseLevel::EraseTileMap(ITileMap * actor)
	{
		auto iter = std::find(_tile_map_actors.begin(), _tile_map_actors.end(), actor);
		if (iter != _tile_map_actors.end())
		{
			std::lock_guard<std::mutex> lock(_tile_map_mutex);
			*iter = _tile_map_actors.back();
			_tile_map_actors.pop_back();
		}
	}

	void IBaseLevel::AddStaticCollider(IColliderBase* collider)
	{
		std::lock_guard<std::mutex> lock(_colliders_mutex);
		_static_colliders.push_back(collider);
	}

	void IBaseLevel::EraseStaticCollider(const IColliderBase* collider)
	{
		auto iter = std::find(_static_colliders.begin(), _static_colliders.end(), collider);
		if (iter != _static_colliders.end())
		{
			std::lock_guard<std::mutex> lock(_colliders_mutex);
			*iter = _static_colliders.back();
			_static_colliders.pop_back();
		}
		
	}

	void IBaseLevel::AddStaticColliders(const std::vector<IColliderBase*>& collider)
	{
		std::lock_guard<std::mutex> lock(_colliders_mutex);
		_static_colliders.insert(_static_colliders.end(), collider.begin(), collider.end());
	}

	void IBaseLevel::AddDynamicCollider(IColliderBase* collider)
	{
		std::lock_guard<std::mutex> lock(_colliders_mutex);
		_dynamic_colliders.push_back(collider);
	}

	void IBaseLevel::EraseDynamicCollider(const IColliderBase* collider)
	{
		auto iter = std::find(_dynamic_colliders.begin(), _dynamic_colliders.end(), collider);
		if (iter != _dynamic_colliders.end())
		{
			std::lock_guard<std::mutex> lock(_colliders_mutex);
			*iter = _dynamic_colliders.back();
			_dynamic_colliders.pop_back();
		}
	}

	void IBaseLevel::AddDynamicColliders(const std::vector<IColliderBase*>&collider)
	{
		std::lock_guard<std::mutex> lock(_colliders_mutex);
		_dynamic_colliders.insert(_dynamic_colliders.end(), collider.begin(), collider.end());
	}

	IThreadPool* IBaseLevel::GetIWindowThreadPool()
	{
		return IEngine::InstancePtr()->GetIWindow()->GetThreadPool();
	}

	void IBaseLevel::_clear()
	{
#ifdef USE_OPENGL
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#endif // USE_OPENGL
	}

	void IBaseLevel::_clear_color() const
	{
#ifdef USE_OPENGL
		glClearColor(_clear_color_vec.r, _clear_color_vec.g, _clear_color_vec.b, _clear_color_vec.a);
#endif // USE_OPENGL
	}

	void IBaseLevel::_collision_detection()
	{
		_colli_handler->StartCollisionHandleDynamic(_static_colliders, _dynamic_colliders);
	}

	void IBaseLevel::_deal_collision()
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
		if (_is_over_collision_detection)
			_collision_detection();
	}

	void IBaseLevel::AddLayer(IEventLayer* layer)
	{
		_event_layers.push_back(layer);
	}

	void IBaseLevel::PopLayer()
	{
		_event_layers.pop_back();
	}

	void IBaseLevel::PopLayer(IEventLayer * layer)
	{
		for (auto iter = _event_layers.begin(); iter != _event_layers.end(); iter++)
		{
			if ((*iter) == layer)
			{
				_event_layers.erase(iter);
			}
		}

	}


	void IBaseLevel::EraseLayer(IEventLayer* layer)
	{
		for (auto iter = layers.begin(); iter != layers.end(); iter++)
		{
			if ((*iter) == layer)
			{
				layers.erase(iter);
			}
		}
		if (layer)
		{
			delete layer;
			layer = nullptr;
		}
	}

}

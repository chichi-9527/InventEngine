#include "IEpch.h"
#include "IBaseLevel.h"
#include "IBaseActor.h"

#include "IEngine.h"
#include "IActor.h"

namespace INVENT
{

	IBaseLevel::IBaseLevel(const glm::vec3& position)
		: IEventLayer()
		, _position(position)
		, _window_size({ 0.0f, 0.0f })
	{
		
		
	}

	IBaseLevel::~IBaseLevel()
	{

		for (auto actor : _all_actors)
		{
			if (actor)
			{
				delete actor;
				actor = nullptr;
			}
		}

	}

	void IBaseLevel::Begin(){}

	void IBaseLevel::Update(float delta)
	{
		for (auto actor : _all_actors)
		{
			if (actor) actor->Update(delta);
		}

		// render imgui
		/*if (_imgui_showing_layer)
		{
			_imgui_showing_layer->RenderImgui();
		}*/

		
	}

	void IBaseLevel::End(){}

	void IBaseLevel::SetPosition(const glm::vec3& position)
	{
		_position = position;
		_recalculate_actors_position();
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

	void IBaseLevel::AddDynamicColliders(const std::vector<IColliderBase*>& collider)
	{
		std::lock_guard<std::mutex> lock(_colliders_mutex);
		_dynamic_colliders.insert(_dynamic_colliders.end(), collider.begin(), collider.end());
	}

	void IBaseLevel::_add2_all_actors(IActor* actor_ptr)
	{
		actor_ptr->SetLevel(this);
		_update_actor_position(actor_ptr);
		actor_ptr->Begin();
		std::lock_guard<std::mutex> lock(_all_actors_mutex);
		_all_actors.emplace_back(actor_ptr);
	}

	void IBaseLevel::_erase_from_all_actors(IActor*& actor_ptr)
	{
		if (!actor_ptr) return;

		std::lock_guard<std::mutex> lock(_all_actors_mutex);
		for (auto& actorPtr : _all_actors)
		{
			if (actorPtr == actor_ptr)
			{
				std::swap(actorPtr, _all_actors.back());
				_all_actors.pop_back();
				actor_ptr = nullptr;
			}
		}
	}

	IThreadPool* IBaseLevel::_get_engine_work_thread_pool()
	{
		return IEngine::InstancePtr()->GetWorkThreadPool();
	}

	void IBaseLevel::_recalculate_actors_position()
	{
		IEngine::InstancePtr()->GetWorkThreadPool()->Submit(0, [this]() {
			for (auto& actor : _all_actors)
			{
				actor->SetWorldPosition(actor->GetWorldPosition() + _position);
			}
			});
		
	}

	void IBaseLevel::_update_actor_position(IActor* actor)
	{
		actor->SetWorldPosition(actor->GetWorldPosition() + _position);
	}

}

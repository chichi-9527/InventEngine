#include "IEpch.h"
#include "IBaseActor.h"
#include "IBaseLevel.h"

namespace INVENT
{
	IBaseActor::~IBaseActor()
	{
		if (_level)
		{
			switch (_collider_type)
			{
			case INVENT::IBaseActor::WorldColliderType::WorldStaticCollider:
				for (auto collider : _colliders)
				{
					this->GetActorFromLevel()->EraseStaticCollider(collider);
				}
				break;
			case INVENT::IBaseActor::WorldColliderType::WorldDynamicCollider:
				for (auto collider : _colliders)
				{
					this->GetActorFromLevel()->EraseDynamicCollider(collider);
				}
				break;
			default:
				INVENT_LOG_ERROR("collider type error");
				break;
			}
		}

	}

	void IBaseActor::Update(float delta)
	{
		for (auto comp : _action_components)
		{
			if (comp)
				comp->Update(delta);
		}
	}

	void IBaseActor::SetLevel(IBaseLevel* level)
	{
		if (nullptr == level)
			return;

		if (_level)
		{
			ClearCollider();
		}
		_level = level;
		switch (_collider_type)
		{
		case INVENT::IBaseActor::WorldColliderType::WorldStaticCollider:
			this->GetActorFromLevel()->AddStaticColliders(_colliders);
			break;
		case INVENT::IBaseActor::WorldColliderType::WorldDynamicCollider:
			this->GetActorFromLevel()->AddDynamicColliders(_colliders);
			break;
		default:
			INVENT_LOG_ERROR("collider type error");
			break;
		}

	}

	void IBaseActor::AddCollider(IColliderBase* base)
	{
		base->SetActorObject(dynamic_cast<IObjectBase*>(this));
		_colliders.push_back(base);

		if (!_level)
			return;

		switch (_collider_type)
		{
		case INVENT::IBaseActor::WorldColliderType::WorldStaticCollider:
			this->GetActorFromLevel()->AddStaticCollider(base);
			break;
		case INVENT::IBaseActor::WorldColliderType::WorldDynamicCollider:
			this->GetActorFromLevel()->AddDynamicCollider(base);
			break;
		default:
			INVENT_LOG_ERROR("collider type error");
			break;
		}
	}

	void IBaseActor::EraseCollider(const IColliderBase * base)
	{
		_colliders.erase(std::remove(_colliders.begin(), _colliders.end(), base), _colliders.end());

		if (!_level)
			return;

		switch (_collider_type)
		{
		case INVENT::IBaseActor::WorldColliderType::WorldStaticCollider:
			this->GetActorFromLevel()->EraseStaticCollider(base);
			break;
		case INVENT::IBaseActor::WorldColliderType::WorldDynamicCollider:
			this->GetActorFromLevel()->EraseDynamicCollider(base);
			break;
		default:
			INVENT_LOG_ERROR("collider type error");
			break;
		}
	}

	void IBaseActor::ClearCollider()
	{
		if (!_level)
		{
			_colliders.clear();
			return;
		}

		switch (_collider_type)
		{
		case INVENT::IBaseActor::WorldColliderType::WorldStaticCollider:
			for (auto collider : _colliders)
			{
				this->GetActorFromLevel()->EraseStaticCollider(collider);
			}
			break;
		case INVENT::IBaseActor::WorldColliderType::WorldDynamicCollider:
			for (auto collider : _colliders)
			{
				this->GetActorFromLevel()->EraseDynamicCollider(collider);
			}
			break;
		default:
			INVENT_LOG_ERROR("collider type error");
			break;
		}
		
	}

	bool IBaseActor::HasCollider()
	{
		return !_colliders.empty();
	}

	bool IBaseActor::HasCollider(const IColliderBase * base)
	{
		return std::find(_colliders.begin(), _colliders.end(), base) != _colliders.end();
	}

	void IBaseActor::UnBindActionComponent(IActionComponent* component)
	{
		for (auto iter = _action_components.begin(); iter != _action_components.end(); iter++)
		{
			if (*iter == component)
				_action_components.erase(iter); return;
		}
	}
}
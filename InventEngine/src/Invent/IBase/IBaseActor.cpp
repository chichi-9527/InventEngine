#include "IEpch.h"
#include "IBaseActor.h"
#include "IBaseLevel.h"

namespace INVENT
{
	void IBaseActor::Update(float delta)
	{
		for (auto comp : _action_components)
		{
			if (comp)
				comp->Update(delta);
		}
	}

	void IBaseActor::AddCollider(IColliderBase* base)
	{
		base->SetActorObject(dynamic_cast<IObjectBase*>(this));
		_colliders.push_back(base);
	}

	void IBaseActor::EraseCollider(const IColliderBase * base)
	{
		_colliders.erase(std::remove(_colliders.begin(), _colliders.end(), base), _colliders.end());
	}

	void IBaseActor::ClearCollider()
	{
		_colliders.clear();
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
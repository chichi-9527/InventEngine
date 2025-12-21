#include "IEpch.h"
#include "IActor.h"

namespace INVENT
{
	IActor::IActor()
		: IObject()
	{

	}

	IActor::~IActor()
	{
		this->ClearColliders();

	}

	void IActor::EraseColliderWithID(AColliderID id)
	{
		this->EraseCollider(_colliders[id.Id]);
		delete _colliders[id.Id];
		_colliders[id.Id] = _colliders.back();
		_colliders.pop_back();
	}

	void IActor::ClearColliders()
	{
		for (auto collider : _colliders)
		{
			this->EraseCollider(collider);
			delete collider;
		}
		_colliders.clear();
	}
}

#include "IEpch.h"
#include "IActor2D.h"

namespace INVENT
{
	IActor2D::IActor2D()
		: IObject2D()
	{

	}

	IActor2D::~IActor2D()
	{
		this->ClearColliders();

	}

	void IActor2D::EraseColliderWithID(AColliderID id)
	{
		this->EraseCollider(_colliders[id.Id]);
		delete _colliders[id.Id];
		_colliders[id.Id] = _colliders.back();
		_colliders.pop_back();
	}

	void IActor2D::ClearColliders()
	{
		for (auto collider : _colliders)
		{
			this->EraseCollider(collider);
			delete collider;
		}
		_colliders.clear();
	}
}
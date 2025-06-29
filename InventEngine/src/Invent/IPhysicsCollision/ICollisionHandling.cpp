#include "ICollisionHandling.h"

#include "ICollisionDetection.h"

#include "IBase/IBaseLevel.h"

namespace INVENT
{
	ICollisionHandling::ICollisionHandling(IBaseLevel* level)
		: _level(level)
		, _tpool(new IThreadPool)
	{
		_tpool->Start();
	}

	ICollisionHandling::~ICollisionHandling()
	{
		if (_tpool)
		{
			delete _tpool;
			_tpool = nullptr;
		}
	}

	void ICollisionHandling::StartCollisionHandleDynamic(const std::vector<IColliderBase*>& static_colliders, const std::vector<IColliderBase*>& dynamic_colliders)
	{
		_tpool->Submit(0, [this, &static_colliders, &dynamic_colliders](){
			StartCollisionHandle(static_colliders, dynamic_colliders);
			});
	}

	void ICollisionHandling::StartCollisionHandle(const std::vector<IColliderBase*>& static_colliders, const std::vector<IColliderBase*>& dynamic_colliders)
	{
		_level->_is_over_collision_detection = false;

		for (auto colider : static_colliders)
		{
			colider->_informations->clear();
		}
		glm::vec3 direction{};
		float distance = 0.0f;
		ICollisionPresets::CollisionType type = ICollisionPresets::CollisionType::COLLISION_BLOCK;
		for (size_t i = 0; i < dynamic_colliders.size(); ++i)
		{
			dynamic_colliders[i]->_informations->clear();

			if (dynamic_colliders[i]->GetCollisionType() == ICollisionPresets::CollisionType::COLLISION_IGNORE)
				continue;
			for (size_t j = i + 1; j < dynamic_colliders.size(); ++j)
			{
				if (dynamic_colliders[j]->GetCollisionType() == ICollisionPresets::CollisionType::COLLISION_IGNORE)
					continue;
				if (ICollisionDetection::IsCollision(dynamic_colliders[i], dynamic_colliders[j], direction, distance))
				{
					
					if (dynamic_colliders[i]->GetCollisionType() == ICollisionPresets::CollisionType::COLLISION_OVERLAP || dynamic_colliders[j]->GetCollisionType() == ICollisionPresets::CollisionType::COLLISION_OVERLAP)
					{
						type = ICollisionPresets::CollisionType::COLLISION_OVERLAP;
					}
					if (dynamic_colliders[i]->_collision_func)
					{
						dynamic_colliders[i]->_informations->emplace_back(IColliderBase::CollisionInformation((IBaseActor*)dynamic_colliders[j]->_object, type, direction, distance));
					}
					if (dynamic_colliders[j]->_collision_func)
					{
						dynamic_colliders[j]->_informations->emplace_back(IColliderBase::CollisionInformation((IBaseActor*)dynamic_colliders[i]->_object, type, -direction, distance));
					}
				}
			}
			for (size_t k = 0; k < static_colliders.size(); ++k)
			{
				if (static_colliders[k]->GetCollisionType() == ICollisionPresets::CollisionType::COLLISION_IGNORE)
					continue;
				if (ICollisionDetection::IsCollision(dynamic_colliders[i], static_colliders[k], direction, distance))
				{

					if (dynamic_colliders[i]->GetCollisionType() == ICollisionPresets::CollisionType::COLLISION_OVERLAP || static_colliders[k]->GetCollisionType() == ICollisionPresets::CollisionType::COLLISION_OVERLAP)
					{
						type = ICollisionPresets::CollisionType::COLLISION_OVERLAP;
					}
					if (dynamic_colliders[i]->_collision_func)
					{
						dynamic_colliders[i]->_informations->emplace_back(IColliderBase::CollisionInformation((IBaseActor*)static_colliders[k]->_object, type, direction, distance));
					}
					if (static_colliders[k]->_collision_func)
					{
						static_colliders[k]->_informations->emplace_back(IColliderBase::CollisionInformation((IBaseActor*)dynamic_colliders[i]->_object, type, -direction, distance));
					}
				}
			}

			if (dynamic_colliders[i]->_collision_func)
			{
				auto collider = dynamic_colliders[i];
				auto collision_callback = [collider]() {
					(collider->_collision_func)(collider->_informations);
					};

				_level->_collider_callbacks.push_back(collision_callback);
			}
		}

		for (size_t k = 0; k < static_colliders.size(); ++k)
		{
			if (static_colliders[k]->_collision_func)
			{
				auto collider = static_colliders[k];
				auto collision_callback = [collider]() {
					(collider->_collision_func)(collider->_informations);
					};
				
				_level->_collider_callbacks.push_back(collision_callback);
			}
		}

		// end
		_level->_is_over_collision_detection = true;

	}
}
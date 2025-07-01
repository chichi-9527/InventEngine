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
			colider->_begin_overlaps.clear();
			colider->_blocks.clear();
			EraseUnorderedSetNotIn2Vectors(static_colliders, dynamic_colliders, colider->_on_overlaps);
			colider->_end_overlaps = colider->_on_overlaps;
		}
		for (auto colider : dynamic_colliders)
		{
			colider->_begin_overlaps.clear();
			colider->_blocks.clear();
			EraseUnorderedSetNotIn2Vectors(static_colliders, dynamic_colliders, colider->_on_overlaps);
			colider->_end_overlaps = colider->_on_overlaps;
		}
		glm::vec3 direction{};
		float distance = 0.0f;
		for (size_t i = 0; i < dynamic_colliders.size(); ++i)
		{
			
			for (size_t j = i + 1; j < dynamic_colliders.size(); ++j)
			{
				auto collision_type = ICollisionHandling::GetCollisionTypeWithTwoCollider(dynamic_colliders[i], dynamic_colliders[j]);
				if (collision_type == ICollisionPresets::CollisionType::COLLISION_IGNORE)
					continue;
				// 获取两个碰撞体的预设，通过预设检测两个碰撞体是否发生碰撞或重叠

				if (ICollisionDetection::IsCollision(dynamic_colliders[i], dynamic_colliders[j], direction, distance))
				{
					if (dynamic_colliders[i]->_object == dynamic_colliders[j]->_object)
						continue;

					if (collision_type == ICollisionPresets::CollisionType::COLLISION_OVERLAP)
					{
						if (IsFindInUnorderedSet(dynamic_colliders[i]->_on_overlaps, dynamic_colliders[j]))
						{
							dynamic_colliders[i]->_end_overlaps.erase(dynamic_colliders[j]);
							dynamic_colliders[j]->_end_overlaps.erase(dynamic_colliders[i]);
						}
						else 
						{
							dynamic_colliders[i]->_begin_overlaps.insert(dynamic_colliders[j]);
							dynamic_colliders[i]->_on_overlaps.insert(dynamic_colliders[j]);

							dynamic_colliders[j]->_begin_overlaps.insert(dynamic_colliders[i]);
							dynamic_colliders[j]->_on_overlaps.insert(dynamic_colliders[i]);
						}
					}
					else
					{
						if (distance)
						{
							_level->_collision_handings.push_back(std::bind(&ICollisionHandling::UpdateBlockActorPosition, dynamic_colliders[i], dynamic_colliders[j], direction, distance));
						}
						dynamic_colliders[i]->_blocks.insert(dynamic_colliders[j]);
						dynamic_colliders[j]->_blocks.insert(dynamic_colliders[i]);
					}

				} // end if IsCollision

			}

			for (size_t k = 0; k < static_colliders.size(); ++k)
			{
				auto collision_type = ICollisionHandling::GetCollisionTypeWithTwoCollider(dynamic_colliders[i], static_colliders[k]);
				if (collision_type == ICollisionPresets::CollisionType::COLLISION_IGNORE)
					continue;
				if (ICollisionDetection::IsCollision(dynamic_colliders[i], static_colliders[k], direction, distance))
				{

					if (collision_type == ICollisionPresets::CollisionType::COLLISION_OVERLAP)
					{
						if (IsFindInUnorderedSet(dynamic_colliders[i]->_on_overlaps, static_colliders[k]))
						{
							dynamic_colliders[i]->_end_overlaps.erase(static_colliders[k]);
							static_colliders[k]->_end_overlaps.erase(dynamic_colliders[i]);
						}
						else
						{
							dynamic_colliders[i]->_begin_overlaps.insert(static_colliders[k]);
							dynamic_colliders[i]->_on_overlaps.insert(static_colliders[k]);

							static_colliders[k]->_begin_overlaps.insert(dynamic_colliders[i]);
							static_colliders[k]->_on_overlaps.insert(dynamic_colliders[i]);
						}
					}
					else
					{
						if (distance)
						{
							_level->_collision_handings.push_back(std::bind(&ICollisionHandling::UpdateBlockActorPosition, dynamic_colliders[i], static_colliders[k], direction, distance));
							dynamic_colliders[i]->_blocks.insert(static_colliders[k]);
							static_colliders[k]->_blocks.insert(dynamic_colliders[i]);
						}
					}
					
				}
			}

			
		}

		auto push2levelcallbacks = [this](IColliderBase* collider) {

			if (collider->_begin_overlap_func && collider->_begin_overlaps.size())
			{
				auto collision_callback = [collider]() {
					(collider->_begin_overlap_func)(collider->_begin_overlaps);
					};

				std::lock_guard<std::mutex> lock(_level->_collision_mutex);
				_level->_collider_callbacks.push_back(collision_callback);
			}
			if (collider->_end_overlaps.size())
			{
				// 结束重叠时删除 正在重叠容器中元素
				for (auto it : collider->_end_overlaps)
					collider->_on_overlaps.erase(it);

				if (collider->_end_overlap_func)
				{
					auto collision_callback = [collider]() {
						(collider->_end_overlap_func)(collider->_end_overlaps);
						};

					std::lock_guard<std::mutex> lock(_level->_collision_mutex);
					_level->_collider_callbacks.push_back(collision_callback);
				}
			}
			if (collider->_block_collision_func && collider->_blocks.size())
			{
				auto collision_callback = [collider]() {
					(collider->_block_collision_func)(collider->_blocks);
					};

				std::lock_guard<std::mutex> lock(_level->_collision_mutex);
				_level->_collider_callbacks.push_back(collision_callback);
			}

			};

		for (size_t i = 0; i < dynamic_colliders.size(); ++i)
		{
			auto collider = dynamic_colliders[i];
			push2levelcallbacks(collider);
		}

		for (size_t i = 0; i < static_colliders.size(); ++i)
		{
			auto collider = static_colliders[i];
			push2levelcallbacks(collider);
		}

		// end
		_level->_is_over_collision_detection = true;

	}

	void ICollisionHandling::UpdateBlockActorPosition(IColliderBase* collider1, IColliderBase* collider2, glm::vec3 direction, float distance)
	{
		
		auto obj1 = collider1->GetActorObject();
		auto obj2 = collider2->GetActorObject();

		if (obj1)
		{
			if (obj2)
			{
				auto actor1 = dynamic_cast<IBaseActor*>(obj1);
				auto actor2 = dynamic_cast<IBaseActor*>(obj2);
				if ((actor1 != nullptr) && (actor2 != nullptr))
				{
					if (actor1->GetWorldColliderType() == IBaseActor::WorldColliderType::WorldStaticCollider)
					{
						obj2->SetWorldPosition(obj2->GetWorldPosition() + distance * direction);
						return;
					}
					else if (actor2->GetWorldColliderType() == IBaseActor::WorldColliderType::WorldStaticCollider)
					{
						obj1->SetWorldPosition(obj1->GetWorldPosition() - distance * direction);
						return;
					}

					// 检测谁能推动谁
					if (actor1->ColiisionLevel == actor2->ColiisionLevel)
					{
						if (actor1->CollisionCoefficient)
						{
							if (actor2->CollisionCoefficient)
							{
								obj1->SetWorldPosition(obj1->GetWorldPosition() - (distance * actor1->CollisionCoefficient / (actor1->CollisionCoefficient + actor2->CollisionCoefficient)) * direction);
								obj2->SetWorldPosition(obj2->GetWorldPosition() + (distance * actor2->CollisionCoefficient / (actor1->CollisionCoefficient + actor2->CollisionCoefficient)) * direction);
								return;
							}
							else
							{
								obj2->SetWorldPosition(obj2->GetWorldPosition() + distance * direction);
								return;
							}
						}
						else
						{
							if (actor2->CollisionCoefficient)
							{
								obj1->SetWorldPosition(obj1->GetWorldPosition() - distance * direction);
								return;
							}
							else
							{
								obj1->SetWorldPosition(obj1->GetWorldPosition() - (distance / 2.0f) * direction);
								obj2->SetWorldPosition(obj2->GetWorldPosition() + (distance / 2.0f) * direction);
								return;
							}
							
						}
					}
					else if (actor1->ColiisionLevel < actor2->ColiisionLevel)
					{
						obj1->SetWorldPosition(obj1->GetWorldPosition() - distance * direction);
						return;
					}
					else
					{
						obj2->SetWorldPosition(obj2->GetWorldPosition() + distance * direction);
						return;
					}

				}
				

				obj1->SetWorldPosition(obj1->GetWorldPosition() - (distance / 2.0f) * direction);
				obj2->SetWorldPosition(obj2->GetWorldPosition() + (distance / 2.0f) * direction);
			}
			else
				obj1->SetWorldPosition(obj1->GetWorldPosition() - distance * direction);
		}
		else
		{
			if (obj2)
			{
				obj2->SetWorldPosition(obj2->GetWorldPosition() + distance * direction);
			}
		}

	}
	const ICollisionPresets::CollisionType& ICollisionHandling::GetCollisionTypeWithTwoCollider(const IColliderBase* collider1, const IColliderBase* collider2)
	{
		
		return ICollisionPresets::GetCollisionTypeWithTwoCollisionPreset(collider1->GetCollisionType(), collider2->GetCollisionType());
	}
}
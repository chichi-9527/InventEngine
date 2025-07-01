#ifndef _ICILLISIONHANDLING_
#define _ICILLISIONHANDLING_

#include "ICollider.h"

#include "ThreadPool/IThreadPool.h"

#include <vector>

namespace INVENT
{
	class IBaseLevel;
	class ICollisionHandling
	{
	public:
		ICollisionHandling(IBaseLevel* level);
		~ICollisionHandling();

		void StartCollisionHandleDynamic(const std::vector<IColliderBase*>& static_colliders, const std::vector<IColliderBase*>& dynamic_colliders);
		void StartCollisionHandle(const std::vector<IColliderBase*>& static_colliders, const std::vector<IColliderBase*>& dynamic_colliders);

		// 碰撞阻挡时逻辑
		static void UpdateBlockActorPosition(IColliderBase* collider1, IColliderBase* collider2, glm::vec3 direction, float distance);

		// 获取碰撞类型
		static const ICollisionPresets::CollisionType& GetCollisionTypeWithTwoCollider(const IColliderBase* collider1, const IColliderBase* collider2);

	private:
		template<typename T>
		inline bool IsFindInVector(const std::vector<T>& vec, T element)
		{
			return std::find(vec.begin(), vec.end(), element) != vec.end();
		}

		template<typename T>
		inline bool IsFindInUnorderedSet(const std::unordered_set<T>& set, T element)
		{
			return set.find(element) != set.end();
		}

		template<typename T>
		inline void FindVector1NotInVector2(const std::vector<T>& in_vec1, const std::vector<T>& in_vec2, std::vector<T>& out_vec)
		{
			std::unordered_set<T> set2(in_vec2.begin(), in_vec2.end());
			for (auto& x : in_vec1)
			{
				if (set2.find(x) == set2.empty())
					out_vec.push_back(x);
			}
		}

		template<typename T>
		inline void EraseUnorderedSetNotIn2Vectors(const std::vector<T>& vec1, const std::vector<T>& vec2, std::unordered_set<T>& set)
		{
			std::unordered_set<T> keep(vec1.begin(), vec1.end());
			keep.insert(vec2.begin(), vec2.end());
			for (auto iter = set.begin(); iter != set.end();)
			{
				if (keep.find(*iter) == keep.end())
					iter = set.erase(iter);
				else
					++iter;
			}
		}

	private:
		IBaseLevel* _level;
		IThreadPool* _tpool;
	};
}

#endif // !_ICILLISIONHANDLING_


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

	private:
		IBaseLevel* _level;
		IThreadPool* _tpool;
	};
}

#endif // !_ICILLISIONHANDLING_


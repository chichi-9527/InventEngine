#ifndef _IActor_
#define _IActor_

#include "IObject.h"
#include "IBaseActor.h"

#include "IPhysicsCollision/ICollider.h"

namespace INVENT
{
	class IActor : public IObject, public IBaseActor
	{
	public:
		IActor();
		virtual ~IActor();

		template<typename T, typename ...Args>
		AColliderID CreateCollider(Args&&...args)
		{
			if (!std::is_base_of_v<IColliderBase, T>)
				return AColliderID();
			_colliders.push_back(new T(std::forward<Args>(args)...));
			this->AddCollider(_colliders.back());
			return AColliderID(_colliders.size() - 1, _colliders.back()->GetColliderType());
		}

		IColliderBase* GetCollider(AColliderID id) { return _colliders[id.Id]; }
		template<typename T>
		T* GetCollider(AColliderID id)
		{
			return dynamic_cast<T*>(GetCollider(id));
		}

		void EraseColliderWithID(AColliderID id);
		void ClearColliders();

	protected:
		std::vector<IColliderBase*> _colliders;
	};

}


#endif // !_IActor


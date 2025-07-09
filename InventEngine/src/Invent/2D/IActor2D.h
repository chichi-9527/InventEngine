#ifndef _IACTOR2D_
#define _IACTOR2D_

#include "IObject2D.h"

#include "IBase/IBaseActor.h"

#include "IPhysicsCollision/ICollider.h"

namespace INVENT
{
	class IActor2D : public IObject2D, public IBaseActor
	{
	public:
		IActor2D();
		virtual ~IActor2D();

		struct AColliderID 
		{
			size_t Id = 0;
			IColliderBase::ColliderType Type = IColliderBase::ColliderType::NONE;

			AColliderID() = default;
			AColliderID(size_t id, IColliderBase::ColliderType type)
				: Id(id)
				, Type(type)
			{}
		};

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

#endif // !_IACTOR2D_


#ifndef _IBASEACTOR_
#define _IBASEACTOR_

#include <vector>

#include "IComponent/InventComponent.h"

#include "IPhysicsCollision/ICollider.h"

#include "IBaseEventFunction.h"

namespace INVENT
{
	class IBaseActor : public IBaseEventFunction
	{
	public:
		enum class WorldColliderType
		{
			WorldStaticCollider,
			WorldDynamicCollider
		};

		IBaseActor() = default;
		virtual ~IBaseActor() = default;

		virtual void Update(float delta);

		virtual void AddCollider(const IColliderBase* base);
		virtual void EraseCollider(const IColliderBase* base);
		virtual void ClearCollider();
		
		// 当 type 为 static 时不检查与其他 static actor 中的碰撞体碰撞
		// 当存在碰撞体时有效，默认 static ,当此 actor 为 pawn（即可控制移动）时返回 dynamic
		// 若想要使 pawn 返回 static，可在pawn中或pawn的派生类中指定 _collider_type
		const WorldColliderType& GetWorldColliderType() const { return _collider_type; }
	protected:
		void BindActionComponent(IActionComponent* component) { _action_components.push_back(component); }
		void UnBindActionComponent(IActionComponent* component);
		const std::vector<IActionComponent*>& GetActionComponents() { return _action_components; }

	protected:
		WorldColliderType _collider_type = WorldColliderType::WorldStaticCollider;

	private:
		std::vector<IActionComponent*> _action_components;
		std::vector<IColliderBase*> _colliders;
	};
}

#endif // !_IBASEACTOR_


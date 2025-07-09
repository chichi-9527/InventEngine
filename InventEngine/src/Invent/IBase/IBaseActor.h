#ifndef _IBASEACTOR_
#define _IBASEACTOR_

#include <vector>

#include "IComponent/InventComponent.h"

#include "IPhysicsCollision/ICollider.h"

#include "IBaseEventFunction.h"

namespace INVENT
{
	class IBaseLevel;
	class IBaseActor : public IBaseEventFunction
	{
	public:
		enum class WorldColliderType
		{
			WorldStaticCollider,
			WorldDynamicCollider
		};

		IBaseActor() = default;
		virtual ~IBaseActor();

		virtual void Update(float delta);

		virtual void AddCollider(IColliderBase* base);
		virtual void EraseCollider(const IColliderBase* base);
		virtual void ClearCollider();
		virtual bool HasCollider();
		virtual bool HasCollider(const IColliderBase* base);
		virtual const std::vector<IColliderBase*>& GetActorColliders() const { return _colliders; }

		inline IBaseLevel* GetActorFromLevel() { return _level; }
		void SetLevel(IBaseLevel* level);
		
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

	public:
		// 碰撞等级，等级低的不能推动等级高的
		unsigned int ColiisionLevel = 0;
		// 碰撞系数，当碰撞等级相同时 通过系数比确定推动距离；
		float CollisionCoefficient = 0.0f;

	private:
		std::vector<IActionComponent*> _action_components;
		std::vector<IColliderBase*> _colliders;

		IBaseLevel* _level = nullptr;
	};
}

#endif // !_IBASEACTOR_


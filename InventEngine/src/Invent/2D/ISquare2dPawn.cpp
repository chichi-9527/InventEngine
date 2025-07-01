#include "IEpch.h"
#include "ISquare2dPawn.h"

namespace INVENT
{
	ISquare2dPawn::ISquare2dPawn()
	{
		this->SetForwardVector({ 0.0f,0.0f,-1.0f });
		this->SetUpVector({ 0.0f,1.0f,0.0f });

		this->_collider_type = WorldColliderType::WorldDynamicCollider;
	}

	ISquare2dPawn::~ISquare2dPawn()
	{}

	void ISquare2dPawn::MoveUp(float delta)
	{
		this->SetWorldPosition(glm::vec3(this->GetWorldPosition().x, this->GetWorldPosition().y + this->GetMoveSpeed() * delta, this->GetWorldPosition().z));
	}

	void ISquare2dPawn::MoveDown(float delta)
	{
		this->SetWorldPosition(glm::vec3(this->GetWorldPosition().x, this->GetWorldPosition().y - this->GetMoveSpeed() * delta, this->GetWorldPosition().z));
	}

	void ISquare2dPawn::MoveLeft(float delta)
	{
		this->SetWorldPosition(glm::vec3(this->GetWorldPosition().x - this->GetMoveSpeed() * delta, this->GetWorldPosition().y, this->GetWorldPosition().z));
	}

	void ISquare2dPawn::MoveRight(float delta)
	{
		this->SetWorldPosition(glm::vec3(this->GetWorldPosition().x + this->GetMoveSpeed() * delta, this->GetWorldPosition().y, this->GetWorldPosition().z));
	}
}
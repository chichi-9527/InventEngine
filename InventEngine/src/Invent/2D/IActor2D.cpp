#include "IEpch.h"
#include "IActor2D.h"

#include "IComponent/InventComponent.h"

namespace INVENT
{
	IActor2D::IActor2D()
		: IActor()
		, _color({ 1.0f, 1.0f,1.0f,1.0f }) // white
	{
		AddComponent<WorldRotationComponent>(glm::vec3{ 0.0f,0.0f,0.0f });
	}

	IActor2D::~IActor2D()
	{
		RemoveComponent<WorldRotationComponent>();
	}

	void IActor2D::SetWorldRotation(const glm::vec3& rotation)
	{
		this->GetComponent<WorldRotationComponent>()->Rotation = rotation;
	}

	const glm::vec3& IActor2D::GetWorldRotation()
	{
		return this->GetComponent<WorldRotationComponent>()->Rotation;
	}

}
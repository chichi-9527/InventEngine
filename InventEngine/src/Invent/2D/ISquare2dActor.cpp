#include "IEpch.h"
#include "ISquare2dActor.h"

namespace INVENT
{
	ISquare2dActor::ISquare2dActor()
		: IObject2D()
	{
		AddComponent<Scale2DComponent>(glm::vec2{ 1.0f,1.0f });
		AddComponent<Rotation2DComponent>(0.0f);

		_shader = IShaderManagement::GetDefaultSquare2DShader();

	}

	ISquare2dActor::~ISquare2dActor()
	{}

	void ISquare2dActor::SetPosition(const glm::vec3 & position)
	{
		auto world_position = this->GetComponent<WorldPositionComponent>();
		if (world_position)
		{
			world_position->WorldPosition = position;
		}
	}

	const glm::vec3& ISquare2dActor::GetPosition()
	{
		return this->GetComponent<WorldPositionComponent>()->WorldPosition;
	}

	void ISquare2dActor::SetScale(const glm::vec2& scale)
	{
		this->GetComponent<Scale2DComponent>()->Scale = scale;
	}

	const glm::vec2& ISquare2dActor::GetScale()
	{
		return this->GetComponent<Scale2DComponent>()->Scale;
	}

	void ISquare2dActor::SetRotation(float rotation)
	{
		this->GetComponent<Rotation2DComponent>()->Rotation = rotation;
	}

	float ISquare2dActor::GetRotation()
	{
		return this->GetComponent<Rotation2DComponent>()->Rotation;
	}
}
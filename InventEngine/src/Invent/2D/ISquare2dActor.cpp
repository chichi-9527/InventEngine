#include "IEpch.h"
#include "ISquare2dActor.h"

#include "IComponent/InventComponent.h"
#include "2DComponent/Invent2DComponent.h"

namespace INVENT
{
	ISquare2dActor::ISquare2dActor()
		: IActor2D()
		, _color({ 1.0f, 1.0f,1.0f,1.0f }) // white
		, _texture_id(0)
		, _texture(nullptr)
	{
		AddComponent<Scale2DComponent>(glm::vec2{ 1.0f,1.0f });
		// AddComponent<Rotation2DComponent>(0.0f);
		AddComponent<WorldRotationComponent>(glm::vec3{ 0.0f,0.0f,0.0f });

		_shader = IShaderManagement::GetDefaultSquare2DShader();

		_texture_coord[0] = { 0.0f, 0.0f };
		_texture_coord[1] = { 1.0f, 1.0f };
		_texture_coord_index.is_valid = false;
	}

	ISquare2dActor::~ISquare2dActor()
	{
		RemoveComponent<Scale2DComponent>();
		RemoveComponent<WorldRotationComponent>();
	}

	/*void ISquare2dActor::SetPosition(const glm::vec3 & position)
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
	}*/

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
		this->GetComponent<WorldRotationComponent>()->Rotation.z = rotation;
	}

	float ISquare2dActor::GetRotation()
	{
		return this->GetComponent<WorldRotationComponent>()->Rotation.z;
	}

	void ISquare2dActor::SetWorldRotation(const glm::vec3& rotation)
	{
		this->GetComponent<WorldRotationComponent>()->Rotation = rotation;
	}

	const glm::vec3& ISquare2dActor::GetWorldRotation()
	{
		return this->GetComponent<WorldRotationComponent>()->Rotation;
	}

	void ISquare2dActor::SetTextureCoord(const glm::vec2& left_down, const glm::vec2& right_up)
	{
		_texture_coord[0] = left_down;
		_texture_coord[1] = right_up;

		_texture_coord_index.is_valid = false;
		_texture_coord_index.width = 0;
		_texture_coord_index.height = 0;
	}

	void ISquare2dActor::SetTextureCoordIndex(unsigned int width, unsigned int height)
	{
		_texture_coord_index.is_valid = true;
		_texture_coord_index.width = width;
		_texture_coord_index.height = height;
	}

	void ISquare2dActor::SetTextureCoordIndex(const ITexture2D::_UInt2 & index)
	{
		_texture_coord_index.is_valid = true;
		_texture_coord_index = index;
	}

}
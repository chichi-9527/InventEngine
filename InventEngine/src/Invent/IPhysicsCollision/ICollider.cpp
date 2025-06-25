#include "ICollider.h"

#include <glm/gtc/matrix_transform.hpp>

namespace INVENT
{
	IColliderBase::IColliderBase(ColliderType type, const glm::vec3& relative_position, IObjectBase* object)
		: _type(type)
		, _object(object)
		, _relative_position(relative_position)
		, _world_position(object ? object->GetWorldPosition() + relative_position : relative_position)
	{
	
	}

	void IColliderBase::SetRelativePosition(const glm::vec3& position)
	{
		_relative_position = position;
		_world_position = _object ? _object->GetWorldPosition() + position : position;
	}

	void IColliderBase::SetWorldPosition(const glm::vec3& position)
	{
		_world_position = position;
		_relative_position = _object ? position - _object->GetWorldPosition() : position;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	IColliderCapsule::IColliderCapsule(IObjectBase* object, const glm::vec3& relative_position, float radius, float height, glm::vec3 rotation)
		: IColliderBase(ColliderType::CAPSULE, relative_position, object)
		, _radius(radius)
		, _height(height)
		, _rotation(rotation)
		, _up({0.0f, 0.0f, 1.0f})
	{
		RecalculateDirection();
	}

	void IColliderCapsule::SetUpVector(const glm::vec3& up)
	{
		_up = up;
		RecalculateDirection();
	}

	void IColliderCapsule::SetRotation(const glm::vec3& rotation)
	{
		_rotation = rotation;
		RecalculateDirection();
	}

	void IColliderCapsule::RecalculateDirection()
	{
		glm::mat4 translate(1.0f);
		if (_rotation.x)
			translate *= glm::rotate(glm::mat4(1.0f), glm::radians(_rotation.x), { 1.0f, 0.0f, 0.0f });
		if (_rotation.y)
			translate *= glm::rotate(glm::mat4(1.0f), glm::radians(_rotation.y), { 0.0f, 1.0f, 0.0f });
		if (_rotation.z)
			translate *= glm::rotate(glm::mat4(1.0f), glm::radians(_rotation.z), { 0.0f, 0.0f, 1.0f });
		_direction = translate * glm::vec4(_up, 1.0f);
		_direction = glm::normalize(_direction);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	IColliderBall::IColliderBall(IObjectBase* object, const glm::vec3& relative_position, float radius)
		: IColliderBase(ColliderType::BALL, relative_position, object)
		, _radius(radius)
	{}

}
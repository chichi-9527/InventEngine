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

	IColliderCapsule::IColliderCapsule(IObjectBase* object, const glm::vec3& relative_position, float radius, float height,const glm::vec3& rotation)
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


	/// <summary>
	/// ///////////////////////////////////////////////////////////////////////////////////////////////////////
	/// </summary>
	

	IColliderBox::IColliderBox(IObjectBase* object, const glm::vec3& relative_position, const glm::vec3& size, const glm::vec3& rotation)
		: IColliderBase(ColliderType::BOX, relative_position, object)
		, _size(size)
		, _rotation(rotation)
		, _vector_X(1.0f,0.0f,0.0f)
		, _vector_Y(0.0f,1.0f,0.0f)
		, _vector_Z(0.0f,0.0f,1.0f)
	{
		SetRotation(rotation);
	}

	void IColliderBox::SetRotation(const glm::vec3& rotation)
	{
		glm::mat4 translate(1.0f);
		if (_rotation.x)
			translate *= glm::rotate(glm::mat4(1.0f), glm::radians(_rotation.x), { 1.0f, 0.0f, 0.0f });
		if (_rotation.y)
			translate *= glm::rotate(glm::mat4(1.0f), glm::radians(_rotation.y), { 0.0f, 1.0f, 0.0f });
		if (_rotation.z)
			translate *= glm::rotate(glm::mat4(1.0f), glm::radians(_rotation.z), { 0.0f, 0.0f, 1.0f });
		_rotated_vector_X = glm::normalize(glm::vec3(translate * glm::vec4(_vector_X, 1.0f)));
		_rotated_vector_Y = glm::normalize(glm::vec3(translate * glm::vec4(_vector_Y, 1.0f)));
		_rotated_vector_Z = glm::normalize(glm::vec3(translate * glm::vec4(_vector_Z, 1.0f)));
	}

	void IColliderBox::SetBaseAxisDirextion(const glm::vec3& x, const glm::vec3& y, const glm::vec3& z)
	{
		_vector_X = x;
		_vector_Y = y;
		_vector_Z = z;

		SetRotation(_rotation);
	}


}
#include "IEpch.h"
#include "ICamera.h"

#include "IEngine.h"

namespace INVENT
{
	ICamera::ICamera()
		: IObjectBase()
		, IBaseActor()
		, IBasePawnControl()
		, _projection_matrix(1.0f)
		, _view_matrix(1.0f)
		, _view_projection_matrix(1.0f)
		, _world_up(0.0f, 0.0f, 1.0f)
		, _rotation_speed(0.001f)
	{
		this->SetForwardVector({ 0.0f,0.0f,-1.0f });
		this->SetUpVector({ 0.0f,1.0f,0.0f });

		this->SetMoveSpeed(0.001f);
	}

	ICamera::~ICamera()
	{}

	void ICamera::SetProjection(float fov, float p_near, float p_far)
	{
		_projection_matrix = glm::perspective(glm::radians(fov), IEngine::InstancePtr()->GetIWindow()->GetWindowAspect(), p_near, p_far);

	}

	void ICamera::MoveForward(float delta)
	{
		this->SetWorldPosition(this->GetWorldPosition() + glm::normalize(this->GetForwardVector()) * delta * this->GetMoveSpeed());
	}

	void ICamera::MoveBackward(float delta)
	{
		this->SetWorldPosition(this->GetWorldPosition() - glm::normalize(this->GetForwardVector()) * delta * this->GetMoveSpeed());
	}

	void ICamera::MoveUp(float delta)
	{
		this->SetWorldPosition(this->GetWorldPosition() + glm::normalize(this->GetUpVector()) * delta * this->GetMoveSpeed());
	}

	void ICamera::MoveDown(float delta)
	{
		this->SetWorldPosition(this->GetWorldPosition() - glm::normalize(this->GetUpVector()) * delta * this->GetMoveSpeed());
	}

	void ICamera::MoveLeft(float delta)
	{
		glm::vec3 left_vector = glm::cross(this->GetUpVector(), this->GetForwardVector());
		this->SetWorldPosition(this->GetWorldPosition() + glm::normalize(left_vector) * delta * this->GetMoveSpeed());
	}

	void ICamera::MoveRight(float delta)
	{
		glm::vec3 right_vector = glm::cross(this->GetForwardVector(), this->GetUpVector());
		this->SetWorldPosition(this->GetWorldPosition() + glm::normalize(right_vector) * delta * this->GetMoveSpeed());
	}

	void ICamera::MoveUpWithWorldUp(float delta)
	{
		this->SetWorldPosition(this->GetWorldPosition() + glm::normalize(_world_up) * delta * this->GetMoveSpeed());
	}

	void ICamera::MoveDownWithWorldUp(float delta)
	{
		this->SetWorldPosition(this->GetWorldPosition() - glm::normalize(_world_up) * delta * this->GetMoveSpeed());
	}

	void ICamera::TurnUp(float delta)
	{
		TurnUpWithAngle(delta * _rotation_speed);
	}

	void ICamera::TurnDown(float delta)
	{
		TurnDownWithAngle(delta * _rotation_speed);
	}

	void ICamera::TurnLeft(float delta)
	{
		TurnLeftWithAngle(delta * _rotation_speed);
	}

	void ICamera::TurnRight(float delta)
	{
		TurnRightWithAngle(delta * _rotation_speed);
	}

	void ICamera::TurnClockwise(float delta)
	{
		TurnClockwiseWithAngle(delta * _rotation_speed);
	}

	void ICamera::TurnCounterclockwise(float delta)
	{
		TurnCounterclockwiseWithAngle(delta * _rotation_speed);
	}

	void ICamera::TurnUpWithAngle(float angle)
	{
		glm::vec3 right_vector = glm::cross(this->GetForwardVector(), this->GetUpVector());
		glm::vec4 rotated_forward_vector = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::normalize(right_vector)) * glm::vec4(this->GetForwardVector(), 1.0f);
		glm::vec4 rotated_up_vector = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::normalize(right_vector)) * glm::vec4(this->GetUpVector(), 1.0f);
		this->SetForwardVector(glm::vec3(rotated_forward_vector));
		this->SetUpVector(glm::vec3(rotated_up_vector));
	}

	void ICamera::TurnDownWithAngle(float angle)
	{
		glm::vec3 left_vector = glm::cross(this->GetUpVector(), this->GetForwardVector());
		glm::vec4 rotated_forward_vector = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::normalize(left_vector)) * glm::vec4(this->GetForwardVector(), 1.0f);
		glm::vec4 rotated_up_vector = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::normalize(left_vector)) * glm::vec4(this->GetUpVector(), 1.0f);
		this->SetForwardVector(glm::vec3(rotated_forward_vector));
		this->SetUpVector(glm::vec3(rotated_up_vector));
	}

	void ICamera::TurnLeftWithAngle(float angle)
	{
		glm::vec4 rotated_forward_vector = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::normalize(this->GetUpVector())) * glm::vec4(this->GetForwardVector(), 1.0f);
		this->SetForwardVector(glm::vec3(rotated_forward_vector));
	}

	void ICamera::TurnRightWithAngle(float angle)
	{
		glm::vec4 rotated_forward_vector = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::normalize(-this->GetUpVector())) * glm::vec4(this->GetForwardVector(), 1.0f);
		this->SetForwardVector(glm::vec3(rotated_forward_vector));
	}

	void ICamera::TurnClockwiseWithAngle(float angle)
	{
		glm::vec4 rotated_up_vector = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::normalize(-this->GetForwardVector())) * glm::vec4(this->GetUpVector(), 1.0f);
		this->SetUpVector(rotated_up_vector);
	}

	void ICamera::TurnCounterclockwiseWithAngle(float angle)
	{
		glm::vec4 rotated_up_vector = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::normalize(this->GetForwardVector())) * glm::vec4(this->GetUpVector(), 1.0f);
		this->SetUpVector(rotated_up_vector);
	}

}
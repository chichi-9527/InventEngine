#ifndef _IBASEPAWNCONTROL_
#define _IBASEPAWNCONTROL_

#include <glm/glm.hpp>

namespace INVENT
{
	class IBasePawnControl2D
	{
	public:
		IBasePawnControl2D() = default;
		virtual ~IBasePawnControl2D() = default;

		void SetMoveSpeed(float speed) { _move_speed = speed; }
		float GetMoveSpeed() const { return _move_speed; }

		void SetForwardVector(const glm::vec3& forward) { _forward = forward; }
		const glm::vec3& GetForwardVector() const { return _forward; }
		glm::vec3 GetForwardVector() { return _forward; }

		void SetUpVector(const glm::vec3& up) { _up = up; }
		const glm::vec3& GetUpVector() const { return _up; }
		glm::vec3 GetUpVector() { return _up; }

		virtual void MoveUp(float delta) = 0;
		virtual void MoveDown(float delta) = 0;
		virtual void MoveLeft(float delta) = 0;
		virtual void MoveRight(float delta) = 0;

	private:
		glm::vec3 _forward = { 0.0f,0.0f,0.0f };
		glm::vec3 _up = { 0.0f,0.0f,0.0f };

		float _move_speed = 0.0f;
	};

	class IBasePawnControl : public IBasePawnControl2D
	{
	public:
		IBasePawnControl() = default;
		virtual ~IBasePawnControl() = default;

		virtual void MoveForward(float delta) = 0;
		virtual void MoveBackward(float delta) = 0;

	};
}

#endif // !_IBASEPAWNCONTROL_


#ifndef _ICAMERA_
#define _ICAMERA_

#include "IObjectBase.h"
#include "IBaseActor.h"
#include "IBasePawnControl.h"

#include <glm/glm.hpp>

namespace INVENT
{
	class ICamera : public IObjectBase,public IBaseActor,public IBasePawnControl
	{
	public:
		ICamera();
		virtual ~ICamera();

		// 转动速度 °/s
		void SetRotationSpeed(float speed) { _rotation_speed = speed; }

		void SetWorldUp(const glm::vec3& vector) { _world_up = vector; }

		// fov: 垂直视野角度
		// p_near: 近裁剪面距离
		// p_far: 远裁剪面距离
		void SetProjection(float fov = 45.0f, float p_near = 0.1f, float p_far = 100.0f);

		virtual void SetWorldPosition(const glm::vec3& position) override;

		// 注意 forward 和 up 的关系
		void SetForwardUpVector(const glm::vec3& forward, const glm::vec3& up);

		const glm::mat4& GetProjectionMatrix() const { return _projection_matrix; }
		const glm::mat4& GetViewMatrix() const { return _view_matrix; }
		const glm::mat4& GetViewProjectionMatrix() const { return _view_projection_matrix; }

		virtual void MoveForward(float delta) override;
		virtual void MoveBackward(float delta) override;
		virtual void MoveUp(float delta) override;
		virtual void MoveDown(float delta) override;
		virtual void MoveLeft(float delta) override;
		virtual void MoveRight(float delta) override;

		virtual void MoveUpWithWorldUp(float delta);
		virtual void MoveDownWithWorldUp(float delta);

		virtual void TurnUp(float delta);
		virtual void TurnDown(float delta);
		virtual void TurnLeft(float delta);
		virtual void TurnRight(float delta);
		virtual void TurnClockwise(float delta);
		virtual void TurnCounterclockwise(float delta);

		virtual void TurnUpWithAngle(float angle);
		virtual void TurnDownWithAngle(float angle);
		virtual void TurnLeftWithAngle(float angle);
		virtual void TurnRightWithAngle(float angle);
		virtual void TurnClockwiseWithAngle(float angle);
		virtual void TurnCounterclockwiseWithAngle(float angle);

		void RecalculateViewMatrix();

	private:
		glm::mat4 _projection_matrix;
		glm::mat4 _view_matrix;
		glm::mat4 _view_projection_matrix;

		glm::vec3 _world_up;

		float _rotation_speed;
	};
}

#endif // !_ICAMERA_

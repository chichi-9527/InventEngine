#ifndef _ICOLLIDER_
#define _ICOLLIDER_

#include "IBase/IObjectBase.h"

#include <glm/glm.hpp>

namespace INVENT
{
	class IColliderBase 
	{
		friend class IColliderCapsule;
		friend class IColliderBall;
	public:
		enum class ColliderType
		{
			CAPSULE,
			BALL,
			BOX
		};

	private:
		IColliderBase(ColliderType type, const glm::vec3& relative_position, IObjectBase* object = nullptr);
	public:

		virtual ~IColliderBase() = default;

		void SetRelativePosition(const glm::vec3& position);
		const glm::vec3& GetRelativePosition() const { return _relative_position; }

		void SetWorldPosition(const glm::vec3& position);
		const glm::vec3& GetWorldPosition() const { return _world_position; }

		const ColliderType& GetColliderType() const { return _type; }

	private:
		ColliderType _type;

		glm::vec3 _relative_position;
		glm::vec3 _world_position;

		IObjectBase* _object;
	};

	class IColliderCapsule : public IColliderBase
	{
	public:
		IColliderCapsule(IObjectBase* object, const glm::vec3& relative_position = { 0.0f,0.0f,0.0f }, float radius = 0.0f, float height = 0.0f, glm::vec3 rotation = { 0.0f,0.0f,0.0f });
		virtual ~IColliderCapsule() = default;

		void SetUpVector(const glm::vec3& up);
		void SetRotation(const glm::vec3& rotation);

		const glm::vec3& GetDirection() const { return _direction; }
		void SetRadius(float radius) { _radius = radius; }
		float GetRadius() const { return _radius; }

	private:
		void RecalculateDirection();

	private:
		
		glm::vec3 _rotation;
		glm::vec3 _up;
		glm::vec3 _direction;
		// 中间圆柱体高度
		float _height;
		float _radius;

	};

	class IColliderBall : public IColliderBase
	{
	public:
		IColliderBall(IObjectBase* object, const glm::vec3& relative_position = { 0.0f,0.0f,0.0f }, float radius = 0.0f);
		virtual ~IColliderBall() = default;

		void SetRadius(float radius) { _radius = radius; }
		float GetRadius() const { return _radius; }

	private:
		float _radius;
	};

}

#endif // !_ICOLLIDER_

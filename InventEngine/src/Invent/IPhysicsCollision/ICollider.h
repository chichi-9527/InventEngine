#ifndef _ICOLLIDER_
#define _ICOLLIDER_

#include "ICollisionPresets.h"

#include "IBase/IObjectBase.h"

#include <glm/glm.hpp>

#include <functional>
#include <memory>

namespace INVENT
{
	class IBaseActor;
	class IColliderBase 
	{
		friend class IColliderCapsule;
		friend class IColliderBall;
		friend class IColliderBox;

		friend class ICollisionHandling;
	public:
		enum class ColliderType
		{
			CAPSULE,
			BALL,
			BOX
		};

		struct CollisionInformation
		{
			CollisionInformation(IBaseActor* act = nullptr
				, ICollisionPresets::CollisionType type = ICollisionPresets::CollisionType::COLLISION_IGNORE
				, const glm::vec3& direct = {}
				, float dist = 0.0f)
				: actor(act)
				, collision_type(type)
				, direction(direct)
				, distance(dist)
			{}
			IBaseActor* actor;
			ICollisionPresets::CollisionType collision_type;
			glm::vec3 direction;
			float distance;
		};

	private:
		IColliderBase(ColliderType type, const glm::vec3& relative_position, IObjectBase* object = nullptr);
	public:
		using CollisionFunction = std::function<void(std::shared_ptr<std::vector<CollisionInformation>>)>;

		virtual ~IColliderBase() = default;

		void SetRelativePosition(const glm::vec3& position);
		const glm::vec3& GetRelativePosition() const { return _relative_position; }

		void SetWorldPosition(const glm::vec3& position);
		const glm::vec3& GetWorldPosition() const { return _world_position; }

		const ColliderType& GetColliderType() const { return _type; }

		void SetCollisionPreset(const ICollisionPresets::CollisionPresets& preset) { _preset = preset; }
		const ICollisionPresets::CollisionType& GetCollisionType() const { return ICollisionPresets::GetCollisionType(_preset); }

		void BindCollisionFunc(CollisionFunction func);

	private:
		ColliderType _type;

		glm::vec3 _relative_position;
		glm::vec3 _world_position;

		ICollisionPresets::CollisionPresets _preset;

		CollisionFunction _collision_func;

		std::shared_ptr<std::vector<CollisionInformation>> _informations;

		IObjectBase* _object;
	};

	class IColliderCapsule : public IColliderBase
	{
	public:
		IColliderCapsule(IObjectBase* object, const glm::vec3& relative_position = { 0.0f,0.0f,0.0f }, float radius = 0.0f, float height = 0.0f, const glm::vec3& rotation = { 0.0f,0.0f,0.0f });
		virtual ~IColliderCapsule() = default;

		void SetUpVector(const glm::vec3& up);
		void SetRotation(const glm::vec3& rotation);

		const glm::vec3& GetDirection() const { return _direction; }
		void SetRadius(float radius) { _radius = radius; }
		float GetRadius() const { return _radius; }

		void SetHeight(float height) { _height = height; }
		float GetHeight() const { return _height; }

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

	class IColliderBox : public IColliderBase
	{
	public:
		IColliderBox(IObjectBase* object, const glm::vec3& relative_position = { 0.0f,0.0f,0.0f }, const glm::vec3& size = { 0.0f,0.0f,0.0f }, const glm::vec3& rotation = { 0.0f,0.0f,0.0f });
		virtual ~IColliderBox() = default;

		void SetSize(const glm::vec3& size) { _size = size; }
		const glm::vec3& GetSize() const { return _size; }

		void SetRotation(const glm::vec3& rotation);
		const glm::vec3& GetRotation() const { return _rotation; }

		// 三个轴的方向，可指定两两不平行的任意方向
		void SetBaseAxisDirextion(const glm::vec3& x, const glm::vec3& y, const glm::vec3& z);
		const glm::vec3& GetAxisDirextionX() const { return _rotated_vector_X; }
		const glm::vec3& GetAxisDirextionY() const { return _rotated_vector_Y; }
		const glm::vec3& GetAxisDirextionZ() const { return _rotated_vector_Z; }

	private:
		// 分别在三个轴上的长度
		glm::vec3 _size;
		glm::vec3 _rotation;

		// 三个轴的方向，可指定两两不平行的任意方向
		glm::vec3 _vector_X;
		glm::vec3 _vector_Y;
		glm::vec3 _vector_Z;

		glm::vec3 _rotated_vector_X;
		glm::vec3 _rotated_vector_Y;
		glm::vec3 _rotated_vector_Z;

	};

}

#endif // !_ICOLLIDER_

#ifndef _ICOLLIDER_
#define _ICOLLIDER_

#include "ICollisionPresets.h"

#include "IBase/IObjectBase.h"

#include <glm/glm.hpp>

#include <functional>
#include <memory>
#include <unordered_set>

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
			NONE,
			CAPSULE,
			BALL,
			BOX
		};

	private:
		IColliderBase(ColliderType type, const glm::vec3& relative_position, IObjectBase* object = nullptr);
	public:
		using CollisionFunction = std::function<void(const std::unordered_set<IColliderBase*>&)>;

		virtual ~IColliderBase() = default;

		void SetRelativePosition(const glm::vec3& position);
		const glm::vec3& GetRelativePosition() const { return _relative_position; }

		void SetWorldPosition(const glm::vec3& position);
		glm::vec3 GetWorldPosition() const;

		const ColliderType& GetColliderType() const { return _type; }

		void SetCollisionPreset(const ICollisionPresets::CollisionPresets& preset) { _preset = preset; }
		const ICollisionPresets::CollisionPresets& GetCollisionType() const { return _preset; }

		template<typename T>
		T GetActorObject() const { return dynamic_cast<T>(_object); }
		IObjectBase* GetActorObject() const { return _object; }
		void SetActorObject(IObjectBase* object) { _object = object; }

		void BindBeginOverlapFunc(CollisionFunction func) { _begin_overlap_func = func; }
		void BindEndOverlapFunc(CollisionFunction func) { _end_overlap_func = func; }
		void BindBlockCollisionFunc(CollisionFunction func) { _block_collision_func = func; }
		void UnBindBeginOverlapFunc() { _begin_overlap_func = nullptr; }
		void UnBindEndOverlapFunc() { _end_overlap_func = nullptr; }
		void UnBindBlockCollisionFunc() { _block_collision_func = nullptr; }

		const std::unordered_set<IColliderBase*>& GetOverlappingColliders() const { return _on_overlaps; }

	private:
		ColliderType _type;

		glm::vec3 _relative_position;

		ICollisionPresets::CollisionPresets _preset;

		CollisionFunction _begin_overlap_func;;
		CollisionFunction _end_overlap_func;
		CollisionFunction _block_collision_func;

		std::unordered_set<IColliderBase*> _on_overlaps;
		std::unordered_set<IColliderBase*> _begin_overlaps;
		std::unordered_set<IColliderBase*> _end_overlaps;
		std::unordered_set<IColliderBase*> _blocks;

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

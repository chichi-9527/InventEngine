#ifndef _INVENTCOMPONENT_
#define _INVENTCOMPONENT_

#include <glm/glm.hpp>

#include <vector>

namespace INVENT
{
	struct IActionComponent 
	{
		IActionComponent() = default;
		IActionComponent(const IActionComponent&) = default;

		virtual void Update(float delta) = 0;
	};

	struct WorldPositionComponent
	{
		glm::vec3 WorldPosition;

		WorldPositionComponent() = default;
		WorldPositionComponent(const WorldPositionComponent&) = default;
		WorldPositionComponent(const glm::vec3& position)
			: WorldPosition(position){}
	};

	struct RelativePositionComponent
	{
		glm::vec3 RelativePosition;

		RelativePositionComponent() = default;
		RelativePositionComponent(const RelativePositionComponent&) = default;
		RelativePositionComponent(const glm::vec3& position)
			: RelativePosition(position){}
	};

	struct RotationComponent
	{
		glm::vec3 Rotation;

		RotationComponent() = default;
		RotationComponent(const RotationComponent&) = default;
		RotationComponent(const glm::vec3& rotation)
			: Rotation(rotation)
		{}
	};

	struct WorldRotationComponent
	{
		glm::vec3 Rotation;

		WorldRotationComponent() = default;
		WorldRotationComponent(const WorldRotationComponent&) = default;
		WorldRotationComponent(const glm::vec3& rotation)
			: Rotation(rotation)
		{}
	};

	struct ScaleComponent
	{
		glm::vec3 Scale;

		ScaleComponent() = default;
		ScaleComponent(const ScaleComponent&) = default;
		ScaleComponent(const glm::vec3& scale)
			: Scale(scale)
		{}
	};

}




#endif // !_INVENTCOMPONENT_

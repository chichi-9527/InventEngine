#ifndef _INVENTCOMPONENT_
#define _INVENTCOMPONENT_

#include <glm/glm.hpp>

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


}




#endif // !_INVENTCOMPONENT_

#ifndef _INVENT2DCOMPONENT_
#define _INVENT2DCOMPONENT_

#include "IComponent/InventComponent.h"

#include <glm/glm.hpp>

namespace INVENT
{
	struct RelativePosition2DComponent
	{
		glm::vec2 RelativePosition;

		RelativePosition2DComponent() = default;
		RelativePosition2DComponent(const RelativePosition2DComponent&) = default;
		RelativePosition2DComponent(const glm::vec2& position)
			: RelativePosition(position){}
	};

	struct Scale2DComponent
	{
		glm::vec2 Scale;

		Scale2DComponent() = default;
		Scale2DComponent(const Scale2DComponent&) = default;
		Scale2DComponent(const glm::vec2& scale)
			: Scale(scale){}
	};

	struct Rotation2DComponent
	{
		float Rotation;

		Rotation2DComponent() = default;
		Rotation2DComponent(const Rotation2DComponent&) = default;
		Rotation2DComponent(const float& rotation)
			: Rotation(rotation){}
	};

}




#endif // !_INVENT2DCOMPONENT_

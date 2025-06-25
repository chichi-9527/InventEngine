#include "IEpch.h"
#include "IObject2D.h"

#include "2DComponent/Invent2DComponent.h"

namespace INVENT
{
	IObject2D::IObject2D()
		: IObjectBase()
		, _parent(nullptr)
	{
		this->AddComponent<RelativePosition2DComponent>(glm::vec2{ 0.0f,0.0f });
	}
	IObject2D::~IObject2D()
	{}



}

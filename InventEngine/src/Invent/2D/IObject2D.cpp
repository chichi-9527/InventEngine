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
	{
		this->RemoveComponent<RelativePosition2DComponent>();
	}

	//void IObject2D::SetParent(IObject2D * parent)
	//{}

	//void IObject2D::AddChild(IObject2D * child)
	//{}

	//void IObject2D::AddChildren(const std::vector<IObject2D*>&children)
	//{}

	//void IObject2D::EraseChild(IObject2D * child)
	//{}

	//void IObject2D::SetWorldPosition(const glm::vec3 & position)
	//{}

	//void IObject2D::SetRelativePosition(const glm::vec2 & position)
	//{}

	//const glm::vec2& IObject2D::GetRelativePosition()
	//{
	//	
	//}



}

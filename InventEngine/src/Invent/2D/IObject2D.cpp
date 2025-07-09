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

	void IObject2D::SetParent(IObject2D * parent)
	{
		_parent = parent;
	}

	void IObject2D::AddChild(IObject2D * child)
	{
		_children.push_back(child);
	}

	void IObject2D::AddChildren(const std::vector<IObject2D*>&children)
	{
		_children.insert(_children.end(), children.begin(), children.end());
	}

	void IObject2D::EraseChild(IObject2D * child)
	{
		_children.erase(std::remove(_children.begin(), _children.end(), child), _children.end());
	}

	void IObject2D::SetWorldPosition(const glm::vec3 & position)
	{
		if (_parent)
		{
			SetRelativePosition(glm::vec2(position - _parent->GetWorldPosition()));
			return;
		}
		IObjectBase::SetWorldPosition(position);
	}

	const glm::vec3& IObject2D::GetWorldPosition()
	{
		if (_parent)
		{
			IObjectBase::SetWorldPosition(_parent->GetWorldPosition() + glm::vec3(GetRelativePosition(), IObjectBase::GetWorldPosition().z));
		}
		return IObjectBase::GetWorldPosition();
	}

	void IObject2D::SetRelativePosition(const glm::vec2 & position)
	{
		this->GetComponent<RelativePosition2DComponent>()->RelativePosition = position;
	}

	const glm::vec2& IObject2D::GetRelativePosition()
	{
		return this->GetComponent<RelativePosition2DComponent>()->RelativePosition;
	}



}

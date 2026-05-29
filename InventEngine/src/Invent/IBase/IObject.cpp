#include "IEpch.h"
#include "IObject.h"

#include "Invent/IComponent/InventComponent.h"

namespace INVENT
{
	IObject::IObject()
		: IObjectBase()
		, _parent(nullptr)
	{
		this->AddComponent<RelativePositionComponent>(glm::vec3{ 0.0f,0.0f,0.0f });
	}

	IObject::~IObject()
	{
		this->RemoveComponent<RelativePositionComponent>();
	}

	void IObject::SetParent(IObject* parent)
	{
		if (parent == _parent || parent == this || std::find(_children.begin(), _children.end(), parent) != _children.end())
		{
			return;
		}

		if (_parent)
		{
			_parent->EraseChild(this);
		}

		_parent = parent;
		if (_parent)
		{
			UpdateRelativePositionAboutParent();
			_parent->AddChild(this);
		}
		UpdateWorldPositionAboutParent();
	}

	void IObject::AddChild(IObject* child)
	{
		if (child->GetParent() != this)
		{
			_children.push_back(child);
			child->SetParent(this);
		}
	}

	void IObject::AddChildren(const std::vector<IObject*>& children)
	{
		for (auto child : children)
		{
			this->AddChild(child);
		}
	}

	void IObject::EraseChild(IObject* child)
	{

		if (child->GetParent() == this)
		{
			for (size_t i = 0; i < _children.size(); ++i)
			{
				if (_children[i] == child)
				{
					_children[i] = _children.back();
					_children.pop_back();
				}
			}
			child->SetParent(nullptr);
		}
		
	}

	void IObject::SetWorldPosition(const glm::vec3 & position)
	{
		IObjectBase::SetWorldPosition(position);

		UpdateRelativePositionAboutParent();

		for (auto child : _children)
		{
			child->UpdateWorldPositionAboutParent();
		}

	}

	void IObject::SetRelativePosition(const glm::vec3& position)
	{
		this->GetComponent< RelativePositionComponent>()->RelativePosition = position;
		UpdateWorldPositionAboutParent();
	}

	const glm::vec3& IObject::GetRelativePosition()
	{
		return this->GetComponent< RelativePositionComponent>()->RelativePosition;
	}

	void IObject::UpdateWorldPositionAboutParent()
	{
		if (_parent)
		{
			IObjectBase::SetWorldPosition(_parent->GetWorldPosition() + GetRelativePosition());
		}
	}

	void IObject::UpdateRelativePositionAboutParent()
	{
		if (_parent)
			this->GetComponent< RelativePositionComponent>()->RelativePosition = GetWorldPosition() - _parent->GetWorldPosition();
	}


}

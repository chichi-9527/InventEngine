#include "IEpch.h"
#include "IObjectBase.h"

namespace INVENT
{
	IComponentManagement IObjectBase::_component_management;

	IObjectBase::IObjectBase()
	{
		_entity = IObjectBase::GetCompomemtManagement().Create();

		AddComponent<WorldPositionComponent>(glm::vec3{ 0.0f });
	}
	IObjectBase::~IObjectBase()
	{}

	IComponentManagement& IObjectBase::GetCompomemtManagement()
	{
		return _component_management;
	}

	void IObjectBase::AddTag(const std::string& tag)
	{
		Tags.push_back(tag);
	}

	void IObjectBase::DeleteTag(const std::string& tag)
	{
		auto iter = std::find(Tags.begin(), Tags.end(), tag);
		if (iter != Tags.end())
			Tags.erase(iter);
	}

	const std::vector<std::string>& IObjectBase::GetTags()
	{
		return Tags;
	}

	bool IObjectBase::FindTag(const std::string& tag)
	{
		for (auto& str : Tags)
		{
			if (str == tag)
				return true;
		}
		return false;
	}

}
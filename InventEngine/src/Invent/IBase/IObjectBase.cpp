#include "IEpch.h"
#include "IObjectBase.h"

#include "IComponent/InventComponent.h"

namespace INVENT
{
	static std::mutex ObjectMutex;

	IObjectBase::IObjectBase()
	{
		_entity = ITools::IDefaultComponentManagement::GetCompomemtManagement().Create();

		AddComponent<WorldPositionComponent>(glm::vec3{ 0.0f });
	}

	IObjectBase::~IObjectBase()
	{
		RemoveComponent<WorldPositionComponent>();
		ITools::IDefaultComponentManagement::GetCompomemtManagement().Remove(_entity);
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

	void IObjectBase::SetWorldPosition(const glm::vec3& position)
	{
		std::lock_guard<std::mutex> lock(ObjectMutex);
		GetComponent<WorldPositionComponent>()->WorldPosition = position;
	}

	const glm::vec3& IObjectBase::GetWorldPosition()
	{
		std::lock_guard<std::mutex> lock(ObjectMutex);
		return GetComponent<WorldPositionComponent>()->WorldPosition;
	}

	std::mutex& IObjectBase::GetObjectMutex()
	{
		return ObjectMutex;
	}

}
#ifndef _IOBJECTBASE_
#define _IOBJECTBASE_

#include <glm/glm.hpp>

#include <vector>
#include <string>

#include "IComponentManagement.h"
#include "ILog.h"
#include "IComponent/InvnetComponent.h"

namespace INVENT
{

	class IObjectBase 
	{
	public:
		IObjectBase();
		virtual ~IObjectBase();

		static IComponentManagement& GetCompomemtManagement();

		void AddTag(const std::string& tag);
		void DeleteTag(const std::string& tag);
		const std::vector<std::string>& GetTags();
		bool FindTag(const std::string& tag);

		void SetWorldPosition(const glm::vec3& position);
		const glm::vec3& GetWorldPosition();

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			INVENT_ASSERT(!HasComponent<T>(), "obj already has component!\n");
			return IObjectBase::GetCompomemtManagement().Emplace<T>(_entity, std::forward<Args>(args)...);
		}

		template<typename T>
		T* GetComponent()
		{
			INVENT_ASSERT(HasComponent<T>(), "obj does not have component!\n");
			return IObjectBase::GetCompomemtManagement().Get<T>(_entity);
		}

		template<typename T>
		bool HasComponent()
		{
			return IObjectBase::GetCompomemtManagement().Has<T>(_entity);
		}

		template<typename T>
		void RemoveComponent()
		{
			INVENT_ASSERT(HasComponent<T>(), "obj does not have component!\n");
			IObjectBase::GetCompomemtManagement().Remove<T>(_entity);
		}

	protected:
		std::vector<std::string> Tags;

	private:
		static IComponentManagement _component_management;

		IComponentManagement::Entity _entity;
	};

}

#endif // !_IOBJECTBASE_


#ifndef _IBASEACTOR_
#define _IBASEACTOR_

#include <vector>

#include "IComponentManagement.h"
#include "IComponent/InvnetComponent.h"

#include "IBaseEventFunction.h"

namespace INVENT
{
	class IBaseActor : public IBaseEventFunction
	{
	public:
		IBaseActor() = default;
		virtual ~IBaseActor() = default;

		virtual void Update(float delta);

	protected:
		void BindActionComponent(IActionComponent* component) { _action_components.push_back(component); }
		void UnBindActionComponent(IActionComponent* component);
		const std::vector<IActionComponent*>& GetActionComponents() { return _action_components; }

	private:
		std::vector<IActionComponent*> _action_components;
	};
}

#endif // !_IBASEACTOR_


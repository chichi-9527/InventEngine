#include "IEpch.h"
#include "IBaseActor.h"

namespace INVENT
{
	void IBaseActor::Update(float delta)
	{
		for (auto comp : _action_components)
		{
			if (comp)
				comp->Update(delta);
		}
	}

	void IBaseActor::UnBindActionComponent(IActionComponent* component)
	{
		for (auto iter = _action_components.begin(); iter != _action_components.end(); iter++)
		{
			if (*iter == component)
				_action_components.erase(iter); return;
		}
	}
}
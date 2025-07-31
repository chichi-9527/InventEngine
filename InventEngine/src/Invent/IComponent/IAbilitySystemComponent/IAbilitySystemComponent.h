#ifndef _IABILITYSYSTEMCOMPONENT_
#define _IABILITYSYSTEMCOMPONENT_

#include "IGameplayAbilitySystem.h"

namespace INVENT
{
	class IAbilitySystemComponent
	{
	public:

		IAbilitySystemComponent();
		IAbilitySystemComponent(const IAbilitySystemComponent&) = default;

		~IAbilitySystemComponent();

		// give ability
		ITagTrie::TrieNodeId GiveAbility();

		

		// give ability and activate once


	};
}

#endif // !_IABILITYSYSTEMCOMPONENT_
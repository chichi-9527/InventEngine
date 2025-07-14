#ifndef _IGAMEPLAYABILITY_
#define _IGAMEPLAYABILITY_

#include "IGASTags.h"

#include <string>

namespace INVENT
{
	class IGameplayAbility 
	{
		friend class IGameplayAbilitySystem;
	public:
		IGameplayAbility();
		virtual ~IGameplayAbility();

		const ITagTrie::TrieNodeId& GetID() const { return _ability_id; }

	private:
		ITagTrie::TrieNodeId _ability_id;
	};
}

#endif // !_IGAMEPLAYABILITY_


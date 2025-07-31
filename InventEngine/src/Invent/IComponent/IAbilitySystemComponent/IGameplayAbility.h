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
		IGameplayAbility() = default;
		virtual ~IGameplayAbility() = default;

		virtual void ActivateAbility(void* pawn) = 0;
		virtual void EndAbility(void* pawn) = 0;

		const ITagTrie::TrieNodeId& GetID() const { return _ability_id; }

	private:
		ITagTrie::TrieNodeId _ability_id;
	};
}

#endif // !_IGAMEPLAYABILITY_


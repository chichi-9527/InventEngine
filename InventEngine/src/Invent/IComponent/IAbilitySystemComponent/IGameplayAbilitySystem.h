#ifndef _IGAMEPLAYABILITYSYSTEM_
#define _IGAMEPLAYABILITYSYSTEM_

#include "IGameplayAbility.h"

#include <unordered_map>

namespace INVENT
{
	class IGameplayAbilitySystem
	{
		IGameplayAbilitySystem();
	public:
		
		~IGameplayAbilitySystem();

		static IGameplayAbilitySystem& Instance();

		IGameplayAbility* FindAbilityWithActiveTag(const std::string& tag);
		IGameplayAbility* FindAbilityWithActiveTag(const ITagTrie::TrieNodeId& id);

		IGameplayAbility* EraseAbilityWithActiveTag(const std::string& tag);
		IGameplayAbility* EraseAbilityWithActiveTag(const ITagTrie::TrieNodeId& id);

		// this function will call : delete ability;
		void DeleteAbilityWithActiveTag(const std::string& tag);
		// this function will call : delete ability;
		void DeleteAbilityWithActiveTag(const ITagTrie::TrieNodeId& id);

		ITagTrie::TrieNodeId AddAbilityWithActiveTag(const std::string& tag, IGameplayAbility* ability);
	private:
		std::unordered_map<size_t, IGameplayAbility*> _ga_map;
	};
}
#endif // !_IGAMEPLAYABILITYSYSTEM_

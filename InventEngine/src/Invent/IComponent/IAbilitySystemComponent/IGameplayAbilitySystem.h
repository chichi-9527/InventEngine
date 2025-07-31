#ifndef _IGAMEPLAYABILITYSYSTEM_
#define _IGAMEPLAYABILITYSYSTEM_

#include "IGameplayAbility.h"

#include "ThreadPool/IThreadPool.h"

#include <unordered_map>

namespace INVENT
{
	class IGameplayAbilitySystem
	{
		IGameplayAbilitySystem();
	public:
		
		~IGameplayAbilitySystem();

		static IGameplayAbilitySystem& Instance();

		// try activate ability
		bool TryActivateAbility(ITagTrie::TrieNodeId id, void* pawn = nullptr);
		// try activate ability by tag
		bool TryActivateAbilityByTag(const std::string& tag, void* pawn = nullptr);

		IGameplayAbility* FindAbilityWithActiveTag(const std::string& tag);
		IGameplayAbility* FindAbilityWithActiveTag(const ITagTrie::TrieNodeId& id);

		IGameplayAbility* EraseAbilityWithActiveTag(const std::string& tag);
		IGameplayAbility* EraseAbilityWithActiveTag(const ITagTrie::TrieNodeId& id);

		// this function will call : delete ability;
		void DeleteAbilityWithActiveTag(const std::string& tag);
		// this function will call : delete ability;
		void DeleteAbilityWithActiveTag(const ITagTrie::TrieNodeId& id);

		ITagTrie::TrieNodeId AddAbilityWithActiveTag(const std::string& tag, IGameplayAbility* ability);

		IThreadPool* GetThreadPool() { return _thread_pool; }
	private:
		std::unordered_map<size_t, IGameplayAbility*> _ga_map;

		IThreadPool* _thread_pool;
	};
}
#endif // !_IGAMEPLAYABILITYSYSTEM_

#include "IEpch.h"
#include "IGameplayAbilitySystem.h"

namespace INVENT
{
	IGameplayAbilitySystem::IGameplayAbilitySystem()
	{

	}

	IGameplayAbilitySystem::~IGameplayAbilitySystem()
	{
		for (auto& kv : _ga_map)
		{
			if (kv.second)
				delete kv.second;
		}
		_ga_map.clear();
	}

	IGameplayAbilitySystem& IGameplayAbilitySystem::Instance()
	{
		static IGameplayAbilitySystem gas;
		return gas;
	}

	IGameplayAbility* IGameplayAbilitySystem::FindAbilityWithActiveTag(const std::string& tag)
	{
		auto tag_id = IGASTags::GetGASTagTrie().Search(tag);
		
		return FindAbilityWithActiveTag(tag_id);
	}

	IGameplayAbility* IGameplayAbilitySystem::FindAbilityWithActiveTag(const ITagTrie::TrieNodeId& id)
	{
		if (_ga_map.find(id.Id) == _ga_map.end())
			return nullptr;
		return _ga_map[id.Id];
	}

	IGameplayAbility* IGameplayAbilitySystem::EraseAbilityWithActiveTag(const std::string& tag)
	{
		auto tag_id = IGASTags::GetGASTagTrie().Erase(tag);
		return EraseAbilityWithActiveTag(tag_id);
	}

	IGameplayAbility* IGameplayAbilitySystem::EraseAbilityWithActiveTag(const ITagTrie::TrieNodeId& id)
	{
		if (id == ITagTrie::TrieNodeId())
			return nullptr;
		auto iter = _ga_map.find(id.Id);
		if (iter == _ga_map.end())
			return nullptr;
		auto ability = iter->second;
		_ga_map.erase(iter);
		return ability;
	}

	void IGameplayAbilitySystem::DeleteAbilityWithActiveTag(const std::string& tag)
	{
		auto ability = EraseAbilityWithActiveTag(tag);
		if (ability)
			delete ability;
	}

	void IGameplayAbilitySystem::DeleteAbilityWithActiveTag(const ITagTrie::TrieNodeId & id)
	{
		auto ability = EraseAbilityWithActiveTag(id);
		if (ability)
			delete ability;
	}

	ITagTrie::TrieNodeId IGameplayAbilitySystem::AddAbilityWithActiveTag(const std::string& tag, IGameplayAbility* ability)
	{
		auto tag_id = IGASTags::GetGASTagTrie().Insert(tag);
		if (_ga_map.find(tag_id.Id) != _ga_map.end())
		{
			INVENT_LOG_WARNING(std::string("this id was have ability : ") + std::to_string(tag_id.Id));
			return ITagTrie::TrieNodeId();
		}
		ability->_ability_id = tag_id;
		_ga_map[tag_id.Id] = ability;
		
		return tag_id;
	}
}
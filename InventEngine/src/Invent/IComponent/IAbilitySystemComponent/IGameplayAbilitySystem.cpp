#include "IEpch.h"
#include "IGameplayAbilitySystem.h"

namespace INVENT
{
	IGameplayAbilitySystem::IGameplayAbilitySystem()
	{
		_entity = ITools::IDefaultComponentManagement::GetCompomemtManagement().Create();

		_ASC = ITools::IDefaultComponentManagement::GetCompomemtManagement().Emplace<IAbilitySystemComponent>(_entity);
	}

	IGameplayAbilitySystem::~IGameplayAbilitySystem()
	{
		ITools::IDefaultComponentManagement::GetCompomemtManagement().Remove<IAbilitySystemComponent>(_entity);
	}
}
#ifndef _IGAMEPLAYABILITYSYSTEM_
#define _IGAMEPLAYABILITYSYSTEM_

#include "IBase/IComponentManagement.h"
#include "IAbilitySystemComponent.h"

namespace INVENT
{
	class IGameplayAbilitySystem
	{
	public:
		IGameplayAbilitySystem();
		virtual ~IGameplayAbilitySystem();

		const IAbilitySystemComponent& GetAbilitySystemComponent() const { return _ASC; }

		void AddGameplayAbility();

	private:
		IComponentManagement::Entity _entity;

		IAbilitySystemComponent _ASC;
	};
}
#endif // !_IGAMEPLAYABILITYSYSTEM_

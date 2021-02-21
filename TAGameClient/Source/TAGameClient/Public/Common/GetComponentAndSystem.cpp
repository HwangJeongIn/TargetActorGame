#include "Common/GetComponentAndSystem.h"

namespace ta
{
	ActorComponent* GetActorComponent(CommonActor* actor, ActorComponentType type) noexcept
	{
		return GetActorComponent(actor->getActorKey(), type);
	}

	ActorComponent* GetActorComponent(const ActorKey& actorKey, ActorComponentType type) noexcept
	{
		ActorComponent* actorComponent = g_app->getActorManager()->getActorComponent(actorKey, type);
		if (false == actorComponent->getComponentValid_())
		{
			return nullptr;
		}

		return (actorComponent);
	}
	
	ActorSystem* GetActorSystem(ActorSystemType type) noexcept
	{
		return (g_app->getActorSystemManager()->getActorSystem(type));
	}
}


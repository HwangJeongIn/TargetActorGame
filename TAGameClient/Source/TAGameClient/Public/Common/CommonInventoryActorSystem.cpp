#include "Common/CommonInventoryActorSystem.h"
#include "Common/CommonInventoryActorComponent.h"


namespace ta
{
	CommonInventoryActorSystem::CommonInventoryActorSystem(void) noexcept
		: ActorSystem(ActorSystemType::InventorySystem)
	{
	}

	CommonInventoryActorSystem::~CommonInventoryActorSystem(void) noexcept
	{
	}

	const ActorSystemType CommonInventoryActorSystem::getActorSystemType(void) noexcept
	{
		return ActorSystemType::InventorySystem;
	}
}


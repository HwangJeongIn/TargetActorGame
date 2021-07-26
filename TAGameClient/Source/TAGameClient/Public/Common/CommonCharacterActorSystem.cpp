#include "Common/CommonCharacterActorSystem.h"
#include "Common/ScopedLock.h"
#include "Common/CommonCharacterActorComponent.h"
#include "Common/GameData.h"


namespace ta
{
	CommonCharacterActorSystem::CommonCharacterActorSystem(void) noexcept
		: ActorSystem(ActorSystemType::CharacterSystem)
	{
	}

	CommonCharacterActorSystem::~CommonCharacterActorSystem(void) noexcept
	{
	}

	const ActorSystemType CommonCharacterActorSystem::getActorSystemType(void) noexcept
	{
		return ActorSystemType::CharacterSystem;
	}
}


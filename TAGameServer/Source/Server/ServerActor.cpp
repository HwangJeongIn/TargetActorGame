#include "Server/ServerActor.h"
#include "ServerActor.h"


namespace ta
{
	ServerActor::ServerActor(void) noexcept
	{
	}

	ServerActor::~ServerActor(void) noexcept
	{
	}

	void ServerActor::onToPool(void) noexcept
	{
		__super::onToPool();
	}

	void ServerActor::onFromPool(const CommonActorBasicSpawnData& spawnData) noexcept
	{
		__super::onFromPool(spawnData);
	}
	
	const CharacterDBNoKey& ServerActor::getCharacterDBNoKey_(void) const noexcept
	{
		return _characterDBNoKey;
	}

	void ServerActor::setCharacterDBNoKey_(const CharacterDBNoKey& characterDBNoKey) noexcept
	{
		_characterDBNoKey = characterDBNoKey;
	}
}


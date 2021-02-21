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
	
	const CharacterDBNo& ServerActor::getCharacterDBNo_(void) const noexcept
	{
		return _characterDBNo;
	}

	void ServerActor::setCharacterDBNo_(const CharacterDBNo& characterDBNo) noexcept
	{
		_characterDBNo = characterDBNo;
	}
}


#include "Client/ClientSpawnDataManager.h"

namespace ta
{
	ClientSpawnDataManager::ClientSpawnDataManager(void) noexcept
	{
	}

	ClientSpawnDataManager::~ClientSpawnDataManager(void) noexcept
	{
	}

	bool ClientSpawnDataManager::initialize(void) noexcept
	{
		if (false == CommonSpawnDataManager::initialize())
		{
			TA_ASSERT_DEV(false, "비정상적인 상황입니다.");
			return false;
		}

		// do initialize

		return true;
	}

	bool ClientSpawnDataManager::open(void) noexcept
	{
		if (false == CommonSpawnDataManager::open())
		{
			TA_ASSERT_DEV(false, "비정상적인 상황입니다.");
			return false;
		}

		// do open

		return true;
	}

	void ClientSpawnDataManager::close(void) noexcept
	{
		// do close

		CommonSpawnDataManager::close();
	}
}



#include "Server/ServerSpawnDataManager.h"
#include "Server/ServerApp.h"
#include "Server/ServerActorManager.h"


namespace ta
{
	ServerSpawnDataManager::ServerSpawnDataManager(void) noexcept
	{
	}

	ServerSpawnDataManager::~ServerSpawnDataManager(void) noexcept
	{
	}

	bool ServerSpawnDataManager::initialize(void) noexcept
	{
		if (false == CommonSpawnDataManager::initialize())
		{
			TA_ASSERT_DEV(false, "비정상적인 상황입니다.");
			return false;
		}

		// do initialize

		return true;
	}

	bool ServerSpawnDataManager::open(void) noexcept
	{
		if (false == CommonSpawnDataManager::open())
		{
			TA_ASSERT_DEV(false, "비정상적인 상황입니다.");
			return false;
		}

		// do open

		return true;
	}

	void ServerSpawnDataManager::close(void) noexcept
	{
		// do close

		CommonSpawnDataManager::close();
	}

	bool ServerSpawnDataManager::spawnWorldActors(const GameWorldType& worldType) noexcept
	{
		std::unordered_map<GameWorldType, std::vector<CommonActorDetailSpawnData*>>::iterator target = _spawnDataSet.find(worldType);
		if (_spawnDataSet.end() == target)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		const std::vector<CommonActorDetailSpawnData*>& spawnActorDataSet = target->second;

		const uint32 count = spawnActorDataSet.size();
		for (uint32 index = 0; index < count; ++index)
		{
			if (nullptr == GetActorManager()->createActorAndInitializeFromSpawnData(*spawnActorDataSet[index]))
			{
				TA_ASSERT_DEV(false, "비정상입니다.");
				return false;
			}
		}

		return true;
	}

	bool ServerSpawnDataManager::spawnRealWorldActors(void) noexcept
	{
		return spawnWorldActors(GameWorldType::RealWorld);
	}
}



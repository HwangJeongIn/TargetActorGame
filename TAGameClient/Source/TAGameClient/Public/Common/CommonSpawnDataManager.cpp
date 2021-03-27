#include "Common/CommonSpawnDataManager.h"


namespace ta
{
#if defined(TA_SERVER) && !defined(TA_CLIENT_IN_SERVER)
	const fs::path SpawnDataFilePath = fs::absolute(L"./../SpawnData");
	//const fs::path SpawnDataFilePathForClientInServer = fs::absolute(L"./../SpawnData");
#elif !defined(TA_SERVER)
	fs::path SpawnDataFilePath = "";
#endif
}

namespace ta
{
	CommonSpawnDataManager::CommonSpawnDataManager(void) noexcept
	{
	}

	CommonSpawnDataManager::~CommonSpawnDataManager(void) noexcept
	{
	}

	bool CommonSpawnDataManager::initialize(void) noexcept
	{
		return true;
	}

	bool CommonSpawnDataManager::open(void) noexcept
	{
		return true;
	}

	void CommonSpawnDataManager::close(void) noexcept
	{

	}
	
	bool CommonSpawnDataManager::loadSpawnDataFromXml(const fs::path& filePath) noexcept
	{
		return true;
	}
}


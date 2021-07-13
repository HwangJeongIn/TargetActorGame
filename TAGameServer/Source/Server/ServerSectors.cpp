#include "Server/ServerSectors.h"
#include "Server/ServerSector.h"
#include "Common/StringUtility.h"
#include "Common/FileLoader.h"
#include "Common/EnumUtility.h"
#include "Common/ThreadTaskManager.h"


namespace ta
{
	ServerSectors::ServerSectors(void) noexcept
	{
	}

	ServerSectors::~ServerSectors(void) noexcept
	{
	}

	bool ServerSectors::initialize(void) noexcept
	{
		if (false == Sectors::initialize())
		{
			TA_ASSERT_DEV(false, "Sectors initialize에 실패했습니다.");
			return false;
		}

		_allSectors = new ServerSector[CountOfSectors];

		//// do initialize
		//std::vector<fs::path> sectorDataFilePaths;
		//if (false == FileLoader::getFilePathsFromDirectory(SectorDataFilePath, spawnDataFilePaths))
		//{
		//	TA_ASSERT_DEV(false, "비정상입니다.");
		//	return false;
		//}

		//const uint32 count = spawnDataFilePaths.size();
		//std::string fileExtention;
		//for (uint32 index = 0; index < count; ++index)
		//{
		//	Extension(spawnDataFilePaths[index].filename().string(), fileExtention);

		//	if (0 == fileExtention.compare("config"))
		//	{
		//		if (false == loadSpawnDataGroupConfig(spawnDataFilePaths[index]))
		//		{
		//			TA_ASSERT_DEV(false, "비정상입니다.");
		//			return false;
		//		}
		//		continue;
		//	}

		//	ThreadTaskLoadSpawnDataFromXml* loadTaskSpawnData = new ThreadTaskLoadSpawnDataFromXml;
		//	loadTaskSpawnData->_spawnDataManager = this;
		//	loadTaskSpawnData->_filePath = spawnDataFilePaths[index];

		//	if (false == RegisterThreadTask(loadTaskSpawnData))
		//	{
		//		TA_ASSERT_DEV(false, "비정상입니다.");
		//		return false;
		//	}
		//}

		//StartRegisteredThreadTasksAndWait();

		if (false == doInitialize())
		{
			TA_ASSERT_DEV(false, "Sectors initialize에 실패했습니다.");
			return false;
		}
		return true;
	}

	bool ServerSectors::open(void) noexcept
	{
		if (false == Sectors::open())
		{
			TA_ASSERT_DEV(false, "Sectors open에 실패했습니다.");
			return false;
		}

		return true;
	}

	void ServerSectors::close(void) noexcept
	{
		Sectors::close();
	}

	Sector* ServerSectors::getSector(const SectorKey& sectorKey) noexcept
	{
		if (false == checkSectorValid(sectorKey))
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return nullptr;
		}

		const int32 index = sectorKey.getKeyValue();
		ServerSector* indexPtr = static_cast<ServerSector*>(_allSectors);

		return &(indexPtr[index]);
	}

}

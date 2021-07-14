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

		// do initialize
		std::vector<fs::path> sectorDataFilePaths;
		if (false == FileLoader::getFilePathsFromDirectory(SectorDataFilePath, sectorDataFilePaths))
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		const uint32 count = sectorDataFilePaths.size();
		std::string fileExtention;
		for (uint32 index = 0; index < count; ++index)
		{
			Extension(sectorDataFilePaths[index].filename().string(), fileExtention);

			if (0 == fileExtention.compare("config"))
			{
				if (false == loadSpawnDataGroupConfig(spawnDataFilePaths[index]))
				{
					TA_ASSERT_DEV(false, "비정상입니다.");
					return false;
				}
				continue;
			}

			ThreadTaskLoadSpawnDataFromXml* loadTaskSpawnData = new ThreadTaskLoadSpawnDataFromXml;
			loadTaskSpawnData->_spawnDataManager = this;
			loadTaskSpawnData->_filePath = spawnDataFilePaths[index];

			if (false == RegisterThreadTask(loadTaskSpawnData))
			{
				TA_ASSERT_DEV(false, "비정상입니다.");
				return false;
			}
		}

		StartRegisteredThreadTasksAndWait();


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

	bool ServerSectors::loadSectorZoneEventDataConfig(const fs::path& filePath) noexcept
	{
		XmlNode rootNode("Root");

		FileLoader::loadXml(filePath, &rootNode);
		const uint32 childElementCount = rootNode.getChildElementCount();
		if (childElementCount != static_cast<uint8>(SectorZoneType::Count) - 1)
		{
			TA_ASSERT_DEV(false, "config에 없는 zone 정보가 존재합니다. config파일을 갱신하세요.");
			return false;
		}

		XmlNode* childElement = nullptr;
		for (uint32 index = 0; index < childElementCount; ++index)
		{
			SectorZoneType currentSectorZoneType = SectorZoneType::Count;

			// ZoneType
			{
				const std::string* value = childElement->getAttribute("ZoneType");
				if (nullptr == value)
				{
					TA_ASSERT_DEV(false, "ZoneType 로드 실패");
					return false;
				}

				currentSectorZoneType = static_cast<SectorZoneType>(FromStringCast<uint8>(*value));
			}


			const uint32 zoneDataPropertyCount = childElement->getChildElementCount();
			for (uint32 zoneDataPropertyIndex = 0; zoneDataPropertyIndex < zoneDataPropertyCount; ++zoneDataPropertyIndex)
			{
				XmlNode* currentProperty = childElement->getChildElement(zoneDataPropertyIndex);
				const std::string& spawnDataGroupName = spawnDataGroup->getName();
				hash_value spawnDataGroupNameHash = ComputeHash(spawnDataGroupName);
				TA_LOG_DEV("RealWorld SpawnDataGroupName : %s, Hash : %lld", ToTstring(spawnDataGroupName).c_str(), spawnDataGroupNameHash);

				std::pair<std::unordered_set<SpawnDataGroupKey>::iterator, bool> rv = _realWorldSpawnGroupList.insert(SpawnDataGroupKey(spawnDataGroupNameHash));
				if (false == rv.second)
				{
					TA_ASSERT_DEV(false, "비정상입니다.");
					return false;
				}
			}


		}

		return true;
	}
}

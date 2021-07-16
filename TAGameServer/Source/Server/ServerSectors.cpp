#include "Server/ServerSectors.h"
#include "Server/ServerSector.h"
#include "Common/StringUtility.h"
#include "Common/FileLoader.h"
#include "Common/EnumUtility.h"
#include "Common/ThreadTaskManager.h"
#include "Common/SectorEventSet.h"
#include "Common/GameData.h"
#include "Common/GetComponentAndSystem.h"


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
		std::vector<SectorZoneMappingData> sectorZoneMappingDataSet;
		sectorZoneMappingDataSet.resize(count - 1); // config파일 제외
		int currentSectorZoneDataIndex = 0;

		for (uint32 index = 0; index < count; ++index)
		{
			Extension(sectorDataFilePaths[index].filename().string(), fileExtention);

			if (0 == fileExtention.compare("config"))
			{
				if (false == loadSectorZoneEventDataConfig(sectorDataFilePaths[index]))
				{
					TA_ASSERT_DEV(false, "비정상입니다.");
					return false;
				}
				continue;
			}

			ThreadTaskLoadSectorEventSetDataFromXml* threadTaskLoadSectorEventSetData = new ThreadTaskLoadSectorEventSetDataFromXml;
			threadTaskLoadSectorEventSetData->_sectors = this; 
			threadTaskLoadSectorEventSetData->_sectorZoneMappingData = &sectorZoneMappingDataSet[currentSectorZoneDataIndex++];
			threadTaskLoadSectorEventSetData->_filePath = sectorDataFilePaths[index];

			if (false == RegisterThreadTask(threadTaskLoadSectorEventSetData))
			{
				TA_ASSERT_DEV(false, "비정상입니다.");
				return false;
			}
		}

		StartRegisteredThreadTasksAndWait();

		// 로드한 SectorZoneData를 저장한다.
		{
			const uint32 count = sectorZoneMappingDataSet.size();
			for (uint32 index = 0; index < count; ++index)
			{
				auto rv = _sectorZoneTypeData.insert(std::pair<SectorKey, SectorZoneType>(sectorZoneMappingDataSet[index]._sectorKey
																						  , sectorZoneMappingDataSet[index]._sectorZoneType));

				if (false == rv.second)
				{
					TA_ASSERT_DEV(false, "비정상입니다. SectorKey : %d", sectorZoneMappingDataSet[index]._sectorKey.getKeyValue());
					return false;
				}
			}
		}

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
		auto it = _sectorZoneEventDataSetData.begin();
		const auto end = _sectorZoneEventDataSetData.end();

		while (end != it)
		{
			delete (it->second);
			++it;
		}

		_sectorZoneEventDataSetData.clear();
		_sectorZoneTypeData.clear();

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

	bool ServerSectors::loadSectorEventSetDataFromXml(const fs::path& filePath, SectorZoneMappingData* sectorZoneMappingData) noexcept
	{
		XmlNode rootNode("Root");

		// xml파일이긴 하지만 SectorZoneType 만 로드 하면되기 때문에 그냥 파일이름을 보고 SectorZoneType을 판단한다.
		// 추후 각 Sector마다 추가 정보가 있을 수 있기 때문에 그때를 위한 xml 포맷은 남겨두기로 한다.
		//if (false == FileLoader::loadXml(filePath, &rootNode))
		//{
		//	TA_ASSERT_DEV(false, "XmlObject생성을 실패했습니다.");
		//	return false;
		//}

		std::string fileName = filePath.filename().string();
		TrimExtension(fileName);

		std::vector<std::string> splitedStrings;
		Split(fileName, "_", splitedStrings, false);
		if (3 != splitedStrings.size())
		{
			TA_ASSERT_DEV(false, "비정상적인 형식입니다. 0_0_Desert와 같은 형식만 지원합니다.");
			return false;
		}

		const SectorZoneType sectorZoneType = ConvertStringToEnum<SectorZoneType>(splitedStrings[2]);
		if (SectorZoneType::Count == sectorZoneType)
		{
			TA_ASSERT_DEV(false, "비정상적인 SectorZoneType 입니다. %s", ToTstring(splitedStrings[2]).c_str());
			return false;
		}

		const int32 indexX = FromStringCast<int32>(splitedStrings[0]);
		const int32 indexY = FromStringCast<int32>(splitedStrings[1]);

		const SectorKey sectorKey = GetSectorKey(indexX, indexY);
		if (false == sectorKey.isValid())
		{
			TA_ASSERT_DEV(false, "비정상적인 SectorKey입니다.");
			return false;
		}

		sectorZoneMappingData->setData(sectorKey, sectorZoneType);


		return true;
	}

	bool ServerSectors::loadSectorZoneEventDataConfig(const fs::path& filePath) noexcept
	{
		XmlNode rootNode("Root");

		FileLoader::loadXml(filePath, &rootNode);
		const uint32 childElementCount = rootNode.getChildElementCount();
		if (childElementCount != static_cast<uint8>(SectorZoneType::Count) - 1)
		{
			TA_LOG_DEV("config와 코드상 zone의 갯수가 일치하지 않습니다 확인해보세요.");
		}

		XmlNode* childElement = nullptr;
		for (uint32 index = 0; index < childElementCount; ++index)
		{
			childElement = rootNode.getChildElement(index);
			SectorZoneType currentSectorZoneType = SectorZoneType::Count;
			// ZoneType
			{
				const std::string* value = childElement->getAttribute("ZoneType");
				if (nullptr == value)
				{
					TA_ASSERT_DEV(false, "ZoneType 로드 실패");
					return false;
				}
				
				currentSectorZoneType = ConvertStringToEnum<SectorZoneType>(*value);
			}

			SectorEventSetData* sectorEventSetData = new SectorEventSetData;
			if (false == loadSectorEventSetData(childElement, sectorEventSetData))
			{
				TA_ASSERT_DEV(false, "SectorEventSetData 로드 실패");
				delete sectorEventSetData;
				return false;
			}

			auto rv = _sectorZoneEventDataSetData.insert(std::pair<SectorZoneType, const SectorEventSetData*>(currentSectorZoneType, sectorEventSetData));
			if (false == rv.second)
			{
				TA_ASSERT_DEV(false, "Config 파일에 SectorZoneType이 여러번 정의되었습니다. %d", static_cast<uint8>(currentSectorZoneType));
				delete sectorEventSetData;
				return false;
			}
		}
		return true;
	}
	
	bool ServerSectors::loadSectorEventSetData(const XmlNode* sectorZoneElement, SectorEventSetData* output) noexcept
	{
		// BasicEventGameDataKeySet = "1,2" SpecialEventGameDataKeySet = "3,4" SpecialEventConditionGameDataKeySet = "3,3"

		// BasicEventGameDataKeySet
		{
			const std::string* value = sectorZoneElement->getAttribute("BasicEventGameDataKeySet");
			if (nullptr == value)
			{
				TA_ASSERT_DEV(false, "BasicEventGameDataKeySet 로드 실패");
				return false;
			}
			std::vector<EventGameDataKeyType> eventGameDataKeyList;
			LoadListFromString<EventGameDataKeyType>(*value, eventGameDataKeyList);
			const uint32 count = eventGameDataKeyList.size();
			for (uint32 index = 0; index < count; ++index)
			{
				const EventGameData* eventGameData = GetGameData<EventGameData>(EventGameDataKey(eventGameDataKeyList[index]));
				if (nullptr == eventGameData)
				{
					TA_ASSERT_DEV(false, "BasicEventGameDataKeySet 로드 실패");
					return false;
				}

				output->_basicEventGameDataSetData.push_back(eventGameData);
			}

		}

		std::vector<const EventGameData*> specialEventGameDataSetData;
		std::vector<const ConditionGameData*> specialConditionGameDataSetData;


		// SpecialEventGameDataKeySet
		{
			const std::string* value = sectorZoneElement->getAttribute("SpecialEventGameDataKeySet");
			if (nullptr == value)
			{
				TA_ASSERT_DEV(false, "SpecialEventGameDataKeySet 로드 실패");
				return false;
			}
			std::vector<EventGameDataKeyType> eventGameDataKeyList;
			LoadListFromString<EventGameDataKeyType>(*value, eventGameDataKeyList);
			const uint32 count = eventGameDataKeyList.size();
			for (uint32 index = 0; index < count; ++index)
			{
				const EventGameData* eventGameData = GetGameData<EventGameData>(EventGameDataKey(eventGameDataKeyList[index]));
				if (nullptr == eventGameData)
				{
					TA_ASSERT_DEV(false, "SpecialEventGameDataKeySet 로드 실패");
					return false;
				}
				specialEventGameDataSetData.push_back(eventGameData);
			}
		}	
		
		
		// SpecialEventConditionGameDataKeySet
		{
			const std::string* value = sectorZoneElement->getAttribute("SpecialEventConditionGameDataKeySet");
			if (nullptr == value)
			{
				TA_ASSERT_DEV(false, "SpecialEventConditionGameDataKeySet 로드 실패");
				return false;
			}

			std::vector<ConditionGameDataKeyType> conditionGameDataKeyList;
			LoadListFromString<ConditionGameDataKeyType>(*value, conditionGameDataKeyList);
			const uint32 count = conditionGameDataKeyList.size();

			for (uint32 index = 0; index < count; ++index)
			{
				const ConditionGameData* conditionGameData = GetGameData<ConditionGameData>(ConditionGameDataKey(conditionGameDataKeyList[index]));
				if (nullptr == conditionGameData)
				{
					TA_ASSERT_DEV(false, "SpecialEventConditionGameDataKeySet 로드 실패");
					return false;
				}
				specialConditionGameDataSetData.push_back(conditionGameData);
			}
		}


		// 최종적으로 컨디션과 이벤트를 Special에 넣어준다.
		{
			const uint32 count = specialEventGameDataSetData.size();
			if (count != specialConditionGameDataSetData.size())
			{
				TA_ASSERT_DEV(false, "SpecialEvent 갯수와 SpecialCondition 갯수가 일치하지 않습니다.");
				return false;
			}

			for (uint32 index = 0; index < count; ++index)
			{
				SpecialSectorEvent* specialSectorEvent = new SpecialSectorEvent(specialEventGameDataSetData[index]
																				, specialConditionGameDataSetData[index]);

				output->_specialEventGameDataSetData.push_back(specialSectorEvent);
			}

		}

		return true;
	}
}

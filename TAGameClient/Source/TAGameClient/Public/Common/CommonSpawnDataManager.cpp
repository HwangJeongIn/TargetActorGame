#include "Common/CommonSpawnDataManager.h"
#include "Common/StringUtility.h"
#include "Common/FileLoader.h"
#include "Common/EnumUtility.h"
#include "Common/ThreadLoadTaskManager.h"
#include "Common/Vector.h"
#include "Common/CommonActor.h"
#include <thread>


namespace ta
{
#ifdef TA_SERVER
	const fs::path SpawnDataPath = fs::absolute(L"./../SpawnData");
	//const fs::path SpawnDataPathForClientInServer = fs::absolute(L"./../SpawnData");
#else
	const fs::path SpawnDataPath = fs::absolute(L"./../../SpawnData");
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
		std::vector<fs::path> spawnDataFilePaths;
		if (false == FileLoader::getFilePathsFromDirectory(SpawnDataPath, spawnDataFilePaths))
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		const uint32 count = spawnDataFilePaths.size();
		for (uint32 index = 0; index < count; ++index)
		{
			ThreadLoadTaskSpawnData* loadTaskSpawnData = new ThreadLoadTaskSpawnData;
			loadTaskSpawnData->_spawnDataManager = this;
			loadTaskSpawnData->_filePath = spawnDataFilePaths[index];

			if (false == RegisterThreadLoadTask(loadTaskSpawnData))
			{
				TA_ASSERT_DEV(false, "비정상입니다.");
				return false;
			}
		}

		StartRegisteredThreadLoadTasksAndWait();

		return true;
	}

	bool CommonSpawnDataManager::open(void) noexcept
	{
		return true;
	}

	void CommonSpawnDataManager::close(void) noexcept
	{
		std::unordered_map<GameWorldType, std::vector<CommonActorDetailSpawnData*>>::iterator it = _spawnDataSet.begin();
		std::unordered_map<GameWorldType, std::vector<CommonActorDetailSpawnData*>>::const_iterator end = _spawnDataSet.end();
		for (; it != end; ++it)
		{
			const uint32 count = it->second.size();
			for (uint32 index = 0; index < count; ++index)
			{
				delete it->second[index];
			}
			it->second.clear();
		}

		_spawnDataSet.clear();
	}
	
	bool CommonSpawnDataManager::loadSpawnDataFromXml(CommonSpawnDataManager* spawnDataManager, const fs::path filePath) noexcept
	{
		XmlNode rootNode;
		if (false == FileLoader::loadXml(filePath, &rootNode))
		{
			TA_ASSERT_DEV(false, "XmlObject생성을 실패했습니다.");
			return false;
		}

		std::string fileName = filePath.filename().string();
		TrimExtension(fileName);
		// 파일이름으로 어떤타입인지 알아낸다.
		const GameWorldType gameWorldType = ConvertStringToEnum<GameWorldType>(fileName);
		std::pair<std::unordered_map<GameWorldType, std::vector<CommonActorDetailSpawnData*>>::iterator, bool> dataSet = 
			spawnDataManager->_spawnDataSet.insert(std::pair(gameWorldType, std::vector<CommonActorDetailSpawnData*>()));
		
		if (false == dataSet.second)
		{
			TA_ASSERT_DEV(false, "중복되는 파일이 있을 수 없는데..");
			return false;
		}

		const uint32 count = rootNode.getChildElementCount();
		XmlNode* childElement = nullptr;
		const std::string* dataString = nullptr;

		GroupGameDataKey groupGameDataKey;
		Vector position;
		Vector rotation;

		for (uint32 index = 0; index < count; ++index)
		{
			childElement = rootNode.getChildElement(index);

			{
				dataString = childElement->getAttribute("GroupGameDataKey");
				if (nullptr == dataString)
				{
					TA_ASSERT_DEV(false, "GroupGameDataKey값이 로드되지 않았습니다. type : %d", static_cast<uint8>(gameWorldType));
					return false;
				}
				groupGameDataKey = Atoi(*dataString);
			}

			{
				dataString = childElement->getAttribute("Position");
				if (nullptr == dataString)
				{
					TA_ASSERT_DEV(false, "Position값이 로드되지 않았습니다. type : %d", static_cast<uint8>(gameWorldType));
					return false;
				}

				if (false == StringWithBracketToVector(*dataString, position))
				{
					TA_ASSERT_DEV(false, "Position String이 비정상입니다. type : %d", static_cast<uint8>(gameWorldType));
					return false;
				}
			}

			{
				dataString = childElement->getAttribute("Rotation");
				if (nullptr == dataString)
				{
					TA_ASSERT_DEV(false, "Rotation값이 로드되지 않았습니다. type : %d", static_cast<uint8>(gameWorldType));
					return false;
				}

				if (false == StringWithBracketToVector(*dataString, rotation))
				{
					TA_ASSERT_DEV(false, "Rotation String이 비정상입니다. type : %d", static_cast<uint8>(gameWorldType));
					return false;
				}
			}

			CommonActorDetailSpawnData* spawnData = new CommonActorDetailSpawnData(position, rotation, groupGameDataKey);
			dataSet.first->second.push_back(spawnData);
		}

		return true;
	}
}


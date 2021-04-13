#include "Server/ServerSpawnDataManager.h"
#include "Server/ServerApp.h"
#include "Server/ServerActorManager.h"
#include "Common/StringUtility.h"
#include "Common/FileLoader.h"
#include "Common/EnumUtility.h"
#include "Common/ThreadLoadTaskManager.h"
#include "Common/Vector.h"
#include "Common/CommonActor.h"
#include "Common/Serializer.h"
#include "Common/ScopedLock.h"
#include <thread>


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
		std::vector<fs::path> spawnDataFilePaths;
		if (false == FileLoader::getFilePathsFromDirectory(SpawnDataFilePath, spawnDataFilePaths))
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		const uint32 count = spawnDataFilePaths.size();
		std::string fileExtention;
		for (uint32 index = 0; index < count; ++index)
		{
			Extension(spawnDataFilePaths[index].filename().string(), fileExtention);

			if (0 == fileExtention.compare("config"))
			{
				if (false == loadSpawnDataGroupConfig(spawnDataFilePaths[index]))
				{
					TA_ASSERT_DEV(false, "비정상입니다.");
					return false;
				}
				continue;
			}

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
		std::unordered_map<GameWorldType, SpawnDataGroupSet>::iterator spawnDataGroupWorldSetIt = _spawnDataGroupWorldSet.begin();
		const std::unordered_map<GameWorldType, SpawnDataGroupSet>::const_iterator spawnDataGroupWorldSetEnd = _spawnDataGroupWorldSet.end();
		for (; spawnDataGroupWorldSetIt != spawnDataGroupWorldSetEnd; ++spawnDataGroupWorldSetIt)
		{
			SpawnDataGroupSet::iterator spawnDataGroupSetIt = spawnDataGroupWorldSetIt->second.begin();
			const SpawnDataGroupSet::const_iterator spawnDataGroupSetEnd = spawnDataGroupWorldSetIt->second.end();
			for (; spawnDataGroupSetIt != spawnDataGroupSetEnd; ++spawnDataGroupSetIt)
			{
				const uint32 spawnDataCount = spawnDataGroupSetIt->second.size();
				for (uint32 spawnDataIndex = 0; spawnDataIndex < spawnDataCount; ++spawnDataIndex)
				{
					delete spawnDataGroupSetIt->second[spawnDataIndex];
				}
				spawnDataGroupSetIt->second.clear();
			}
			spawnDataGroupWorldSetIt->second.clear();
		}

		_spawnDataGroupWorldSet.clear();

		CommonSpawnDataManager::close();
	}

	bool ServerSpawnDataManager::loadSpawnDataFromXml(const fs::path& filePath) noexcept
	{
		XmlNode rootNode("Root");
		if (false == FileLoader::loadXml(filePath, &rootNode))
		{
			TA_ASSERT_DEV(false, "XmlObject생성을 실패했습니다.");
			return false;
		}

		std::string fileName = filePath.filename().string();
		TrimExtension(fileName);
		
		hash_value fileNameHash = ComputeHash(fileName);

		std::pair<SpawnDataGroupKey, std::vector<CommonActorDetailSpawnDataForCharacter*>> spawnDataGroupPair;
		spawnDataGroupPair.first = fileNameHash;

		const uint32 childElementCount = rootNode.getChildElementCount();
		XmlNode* childElement = nullptr;
		XmlNode* grandChildElement = nullptr;

		GroupGameDataKey groupGameDataKey;
		Vector position;
		Vector rotation;

		for (uint32 index = 0; index < childElementCount; ++index)
		{
			childElement = rootNode.getChildElement(index);

			const uint32 grandChildElementCount = childElement->getChildElementCount();
			for (uint32 grandChildElementIndex = 0; grandChildElementIndex < grandChildElementCount; ++grandChildElementIndex)
			{
				grandChildElement = childElement->getChildElement(grandChildElementIndex);
				if (0 == grandChildElement->getName().compare("GroupGameData"))
				{
					groupGameDataKey = FromStringCast<GroupGameDataKeyType>(*(grandChildElement->getAttribute("Key")));
				}
				else if (0 == grandChildElement->getName().compare("Position"))
				{
					position._x = FromStringCast<float>(*(grandChildElement->getAttribute("X")));
					position._y = FromStringCast<float>(*(grandChildElement->getAttribute("Y")));
					position._z = FromStringCast<float>(*(grandChildElement->getAttribute("Z")));
				}
				else if (0 == grandChildElement->getName().compare("Rotation"))
				{
					rotation._x = FromStringCast<float>(*(grandChildElement->getAttribute("X")));
					rotation._y = FromStringCast<float>(*(grandChildElement->getAttribute("Y")));
					rotation._z = FromStringCast<float>(*(grandChildElement->getAttribute("Z")));
				}
			}

			CommonActorDetailSpawnDataForCharacter* spawnData = new CommonActorDetailSpawnDataForCharacter(position, rotation, groupGameDataKey);
			spawnDataGroupPair.second.push_back(spawnData);
		}

		
		{
			ScopedLock lock(&_spawnDataGroupWorldSetLoadLock);

			GameWorldType gameWorldType = GameWorldType::RealWorld;
			if (_realWorldSpawnGroupList.end() == _realWorldSpawnGroupList.find(spawnDataGroupPair.first))
			{
				gameWorldType = GameWorldType::TempWorld;
			}

			std::pair<std::unordered_map<GameWorldType, SpawnDataGroupSet>::iterator, bool> worldSet = _spawnDataGroupWorldSet.insert(std::pair(gameWorldType, SpawnDataGroupSet()));
			std::pair<SpawnDataGroupSet::iterator, bool> rv = worldSet.first->second.insert(spawnDataGroupPair);
			if (false == rv.second)
			{
				TA_ASSERT_DEV(false, "중복되는 파일이 있을 수 없는데..");
				return false;
			}
		}

		return true;
	}

	bool ServerSpawnDataManager::loadSpawnDataGroupConfig(const fs::path& filePath) noexcept
	{
		XmlNode rootNode("Root");

		FileLoader::loadXml(filePath, &rootNode);
		const uint32 childElementCount = rootNode.getChildElementCount();
		if (1 != childElementCount)
		{
			TA_ASSERT_DEV(false, "아직 지원하지 않습니다.");
			return false;
		}

		XmlNode* childElement = nullptr;
		for (uint32 index = 0; index < childElementCount; ++index)
		{
			childElement = rootNode.getChildElement(index);
			if ("RealWorld" == childElement->getName())
			{
				const uint32 realWorldSpawnGroupDataCount = childElement->getChildElementCount();
				for (uint32 realWorldSpawnGroupDataIndex = 0; realWorldSpawnGroupDataIndex < realWorldSpawnGroupDataCount; ++realWorldSpawnGroupDataIndex)
				{
					XmlNode* spawnDataGroup = childElement->getChildElement(realWorldSpawnGroupDataIndex);
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
		}

		return true;
	}

	bool ServerSpawnDataManager::spawnWorldActors(const GameWorldType& worldType) noexcept
	{
		std::unordered_map<GameWorldType, SpawnDataGroupSet>::iterator target = _spawnDataGroupWorldSet.find(worldType);
		if (_spawnDataGroupWorldSet.end() == target)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		SpawnDataGroupSet::iterator spawnDataGroupSetIt = target->second.begin();
		const SpawnDataGroupSet::const_iterator spawnDataGroupSetEnd = target->second.end();
		for (; spawnDataGroupSetIt != spawnDataGroupSetEnd; ++spawnDataGroupSetIt)
		{
			const uint32 spawnDataCount = spawnDataGroupSetIt->second.size();
			for (uint32 spawnDataIndex = 0; spawnDataIndex < spawnDataCount; ++spawnDataIndex)
			{
				if (nullptr == GetActorManager()->createCharacterActorAndInitializeComponentsFromSpawnData(*spawnDataGroupSetIt->second[spawnDataIndex]))
				{
					TA_ASSERT_DEV(false, "비정상입니다.");
					return false;
				}
			}
		}

		return true;
	}

	bool ServerSpawnDataManager::spawnRealWorldActors(void) noexcept
	{
		return spawnWorldActors(GameWorldType::RealWorld);
	}
}



#include "Common/GameDataManager.h"
#include "Common/StringUtility.h"
#include "Common/FileLoader.h"
#include "Common/GetComponentAndSystem.h"
#include "Common/EnumUtility.h"
#include "Common/ThreadLoadTaskManager.h"
#include <thread>
#include <sstream>


namespace ta
{
#ifdef TA_SERVER
	const fs::path GameDataXmlFilePath = fs::absolute(L"./../../TAGameClient/GameData/Xml");
#else
	fs::path GameDataXmlFilePath = "";
#endif
}


namespace ta
{
	GameDataManager::GameDataManager(void) noexcept
	{

	}
	
	GameDataManager::~GameDataManager(void) noexcept
	{
	}

	bool GameDataManager::initialize(void) noexcept
	{
		std::vector<fs::path> xmlFiles;

//#ifdef TA_SERVER
		if (false == FileLoader::getFilePathsFromDirectory(GameDataXmlFilePath, xmlFiles))
//#else
//		if (false == FileLoader::getFilePathsFromDirectory(*FPaths::GetProjectFilePath(), xmlFiles))
//#endif
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		//std::vector<XmlNode*> rootList; // 인덱스 = GameDataType값
		//rootList.resize(static_cast<uint8>(GameDataType::Count), nullptr);

		const uint32 count = xmlFiles.size();
		for (uint32 index = 0; index < count; ++index)
		{
			ThreadLoadTaskGameData* loadTaskGameData = new ThreadLoadTaskGameData;
			loadTaskGameData->_gameDataManager = this;
			loadTaskGameData->_filePath = xmlFiles[index];

			if (false == RegisterThreadLoadTask(loadTaskGameData))
			{
				TA_ASSERT_DEV(false, "비정상입니다.");
				return false;
			}
		}

		StartRegisteredThreadLoadTasksAndWait();

		return true;
	}

	bool GameDataManager::open(void) noexcept
	{
		return true;
	}

	void GameDataManager::close(void) noexcept
	{
#define CLOSE_GAMEDATA(GameDataName)												  								\
		{																											\
			std::unordered_map<GameDataName##Key, GameDataName*>::iterator it = _setOf##GameDataName.begin();		\
			const std::unordered_map<GameDataName##Key, GameDataName *>::iterator end = _setOf##GameDataName.end();	\
			for (; it != end; ++it)																					\
			{																										\
				if (nullptr != it->second)																			\
				{																									\
					delete it->second;																				\
					it->second = nullptr;																			\
				}																									\
			}																										\
			_setOf##GameDataName.clear();																			\
		}																											\


		CLOSE_GAMEDATA(GroupGameData)
		CLOSE_GAMEDATA(MoveGameData)
		CLOSE_GAMEDATA(AiGameData)
		CLOSE_GAMEDATA(CharacterGameData)
		CLOSE_GAMEDATA(ItemGameData)

		TA_COMPILE_DEV(5 == static_cast<uint8>(GameDataType::Count), "여기도 확인해주세요")
#undef CLOSE_GAMEDATA
	}
	
	const GameData* GameDataManager::getGameData(const int32 key, const GameDataType& gameDataType) const noexcept
	{
		switch (gameDataType)
		{

#define RETURN_GAMEDATA(GameDataName)																								 \
		case GameDataType::GameDataName:																							 \
			{																														 \
				const std::unordered_map<GameDataName##Key, GameDataName *>::const_iterator target 									 \
								= _setOf##GameDataName.find(GameDataName##Key(key));												 \
				const std::unordered_map<GameDataName##Key, GameDataName *>::const_iterator end = _setOf##GameDataName.end();		 \
																																	 \
				if (end == target)																									 \
				{																													 \
					TA_LOG_DEV("데이터가 없습니다. type : %d, key : %d", static_cast<uint32>(gameDataType), key);						 \
					return nullptr;																									 \
				}																													 \
																																	 \
				if (false == target->second->isValid())																				 \
				{																													 \
					TA_ASSERT_DEV(false, "비정상적인 데이터입니다.");																	 \
					return nullptr;																									 \
				}																													 \
																																	 \
				return (target->second);																							 \
			}																														 \

		RETURN_GAMEDATA(GroupGameData)
		RETURN_GAMEDATA(MoveGameData)
		RETURN_GAMEDATA(AiGameData)
		RETURN_GAMEDATA(CharacterGameData)
		RETURN_GAMEDATA(ItemGameData)

		TA_COMPILE_DEV(5 == static_cast<uint8>(GameDataType::Count), "여기도 확인해주세요")
#undef RETURN_GAMEDATA
		
		default:
			{
				TA_ASSERT_DEV(false, "비정상입니다.")
			}
			break;
		}

		return nullptr;
	}

	bool GameDataManager::loadGameDataFromXml(const fs::path filePath) noexcept
	{
		XmlNode rootNode("Root");
		if (false == FileLoader::loadXml(filePath, &rootNode))
		{
			TA_ASSERT_DEV(false, "XmlObject생성을 실패했습니다.");
			return false;
		}

		// xml save test
		if (false)
		{
			fs::path newFilePath = filePath;
			newFilePath += ".test.xml";
			FileLoader::saveXml(newFilePath, &rootNode);
		}

		std::string fileName = filePath.filename().string();
		TrimExtension(fileName);
		// 파일이름으로 어떤타입인지 알아낸다.
		const GameDataType gameDataType = ConvertStringToEnum<GameDataType>(fileName);

		const uint32 count = rootNode.getChildElementCount();
		XmlNode* childElement = nullptr;
		const std::string* keyString = nullptr;

		//const std::unordered_map<std::string, XmlNode*>& childElements = rootNode.getChildElements();
		//std::unordered_map<std::string, XmlNode*>::const_iterator it = childElements.begin();
		//const std::unordered_map<std::string, XmlNode*>::const_iterator end = childElements.end();

		for(uint32 index =0; index <count; ++index)
		{
			childElement = rootNode.getChildElement(index);
			keyString = childElement->getAttribute("Key");
			if (nullptr == keyString)
			{
				TA_ASSERT_DEV(false, "Key값이 로드되지 않았습니다. type : %d" , static_cast<uint8>(gameDataType));
				return false;
			}

			std::wstringstream ss;
			ss << std::this_thread::get_id();
			//std::thread::id this_id = std::this_thread::get_id();
			//size_t threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
			

			TA_LOG_DEV("GameDataType : %d, Thread id : %s", static_cast<uint32>(gameDataType), ss.str().c_str());
			
			//std::cout << "GameDataType : " << static_cast<uint32>(gameDataType) << ", Thread id : " << this_id << std::endl;
			switch (gameDataType)
			{
				// _setOf##GameDataName 이 각각의 변수니까 공유하지 않는다 따로 락걸필요가 없다.

#define LOADXML(GameDataName)																				\
			case GameDataType::GameDataName:																\
				{																							\
					TA_LOG_DEV("Switch GameDataType : %d, Thread id : %s", static_cast<uint32>(GameDataType::GameDataName), ss.str().c_str());\
					const GameDataName##Key key(FromStringCast<int32>(*keyString));							\
					GameDataName* gameData = new GameDataName;												\
					gameData->clear();																		\
					gameData->_key = key;																	\
					if (false == gameData->loadXml(childElement))											\
					{																						\
						TA_ASSERT_DEV(false, "게임데이터 loadXml에 실패했습니다. type : %d, key : %d"			\
									  , static_cast<uint8>(gameDataType)									\
									  , key.getKeyValue());													\
																											\
						return false;																		\
					}																						\
					gameData->setIsValid(true);																\
																											\
					std::pair<std::unordered_map<GameDataName##Key, GameDataName *>::iterator, bool> rv		\
						= _setOf##GameDataName.insert(std::make_pair(key, gameData));					    \
																											\
					if (false == rv.second)																	\
					{																						\
						TA_ASSERT_DEV(false, "Key값 중복됩니다.type : %d, key : %d"						    \
									  , static_cast<uint8>(gameDataType)									\
									  , key.getKeyValue());													\
																											\
						return false;																		\
					}																						\
				}																							\
				break;																						\


				LOADXML(GroupGameData)
				LOADXML(MoveGameData)
				LOADXML(AiGameData)
				LOADXML(CharacterGameData)
				LOADXML(ItemGameData)

				TA_COMPILE_DEV(5 == static_cast<uint8>(GameDataType::Count), "여기도 확인해주세요")
#undef LOADXML

			default:
				{

				}
				break;
			}
		}
		return true;
	}
}


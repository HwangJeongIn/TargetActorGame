#include "Common/GameDataManager.h"
#include "Common/StringUtility.h"
#include "Common/FileLoader.h"
#include "Common/GetComponentAndSystem.h"
#include "Common/EnumUtility.h"
#include "Common/ThreadTaskManager.h"
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

		std::vector<std::vector<GameDataLoadHelper*>> gameDataLoadHelperSetGroup;
		{ /* LoadFromXml */ }
		{
			std::vector<fs::path> xmlFiles;

			const uint8 gameDataTypeCount = static_cast<uint8>(GameDataType::Count);
			gameDataLoadHelperSetGroup.resize(gameDataTypeCount);

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
				ThreadTaskLoadGameDataFromXml* threadTaskLoadGameDataFromXml = new ThreadTaskLoadGameDataFromXml;
				threadTaskLoadGameDataFromXml->_gameDataManager = this;
				threadTaskLoadGameDataFromXml->_filePath = xmlFiles[index];
				threadTaskLoadGameDataFromXml->_gameDataLoadHelperSetGroup = &gameDataLoadHelperSetGroup;
				if (false == RegisterThreadTask(threadTaskLoadGameDataFromXml))
				{
					TA_ASSERT_DEV(false, "비정상입니다.");
					return false;
				}
			}

			StartRegisteredThreadTasksAndWait();
		}

		{ /* FinishLoading */ }
		{
			const uint32 count = gameDataLoadHelperSetGroup.size();
			for (uint32 index = 0; index < count; ++index)
			{
				const GameDataType currentGameDataType = static_cast<GameDataType>(index);
				ThreadTaskFinishGameDataLoading* finishGameDataLoadingTask = new ThreadTaskFinishGameDataLoading;
				finishGameDataLoadingTask->_gameDataManager = this;
				finishGameDataLoadingTask->_gameDataType = currentGameDataType;
				finishGameDataLoadingTask->_gameDataLoadHelperSet = &gameDataLoadHelperSetGroup[index];
				if (false == RegisterThreadTask(finishGameDataLoadingTask))
				{
					TA_ASSERT_DEV(false, "비정상입니다.");
					return false;
				}
			}

			StartRegisteredThreadTasksAndWait();

			// 사용했던 Helper 클래스의 메모리를 지워준다.
			{
				const uint32 helperSetGroupSize = gameDataLoadHelperSetGroup.size();
				for (uint32 setGroupIndex = 0; setGroupIndex < helperSetGroupSize; ++setGroupIndex)
				{
					const uint32 helperSetSize = gameDataLoadHelperSetGroup[setGroupIndex].size();
					for (uint32 setIndex = 0; setIndex < helperSetSize; ++setIndex)
					{
						delete gameDataLoadHelperSetGroup[setGroupIndex][setIndex];
					}
				}

				gameDataLoadHelperSetGroup.clear();
			}
		}

		{ /* CheckFinally */ }
		{			
			const uint8 gameDataTypeCount = static_cast<uint8>(GameDataType::Count);
			for (uint32 index = 0; index < gameDataTypeCount; ++index)
			{
				const GameDataType currentGameDataType = static_cast<GameDataType>(index);
				ThreadTaskCheckGameDataFinally* checkGameDataFinallyTask = new ThreadTaskCheckGameDataFinally;
				checkGameDataFinallyTask->_gameDataManager = this;
				checkGameDataFinallyTask->_gameDataType = currentGameDataType;
				if (false == RegisterThreadTask(checkGameDataFinallyTask))
				{
					TA_ASSERT_DEV(false, "비정상입니다.");
					return false;
				}
			}

			StartRegisteredThreadTasksAndWait();
		}

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
		CLOSE_GAMEDATA(RenderingGameData)
		CLOSE_GAMEDATA(ConditionGameData)
		CLOSE_GAMEDATA(EventGameData)
		CLOSE_GAMEDATA(BuffGameData)

		TA_COMPILE_DEV(9 == static_cast<uint8>(GameDataType::Count), "여기도 확인해주세요")
#undef CLOSE_GAMEDATA
	}
	

	bool GameDataManager::loadGameDataFromXml(const fs::path filePath, std::vector<std::vector<GameDataLoadHelper*>>* gameDataLoadHelperSet) noexcept
	{
		XmlNode rootNode("Root");
		if (false == FileLoader::loadXml(filePath, &rootNode))
		{
			TA_ASSERT_DEV(false, "XmlObject생성을 실패했습니다.");
			return false;
		}

		//// xml save test
		//if (false)
		//{
		//	fs::path newFilePath = filePath;
		//	newFilePath += ".test.xml";
		//	FileLoader::saveXml(newFilePath, &rootNode);
		//}

		std::string fileName = filePath.filename().string();
		TrimExtension(fileName);
		// 파일이름으로 어떤타입인지 알아낸다.
		const GameDataType gameDataType = ConvertStringToEnum<GameDataType>(fileName);
		if (GameDataType::Count == gameDataType)
		{
			TA_ASSERT_DEV(false, "비정상");
			return false;
		}

		std::vector<GameDataLoadHelper*>& gameDataLoadHelpers = gameDataLoadHelperSet->at(static_cast<uint8>(gameDataType));
		const uint32 count = rootNode.getChildElementCount();
		XmlNode* childElement = nullptr;
		const std::string* keyString = nullptr;

		//const std::unordered_map<std::string, XmlNode*>& childElements = rootNode.getChildElements();
		//std::unordered_map<std::string, XmlNode*>::const_iterator it = childElements.begin();
		//const std::unordered_map<std::string, XmlNode*>::const_iterator end = childElements.end();

		std::wstringstream ss;
		ss << std::this_thread::get_id();
		TA_LOG_DEV("[LoadFromXml] => GameDataType : %d, Thread id : %s", static_cast<uint32>(gameDataType), ss.str().c_str());

		for (uint32 index = 0; index < count; ++index)
		{
			childElement = rootNode.getChildElement(index);
			keyString = childElement->getAttribute("Key");
			if (nullptr == keyString)
			{
				TA_ASSERT_DEV(false, "Key값이 로드되지 않았습니다. type : %d", static_cast<uint8>(gameDataType));
				return false;
			}

			//std::cout << "GameDataType : " << static_cast<uint32>(gameDataType) << ", Thread id : " << this_id << std::endl;
			switch (gameDataType)
			{
				// _setOf##GameDataName 이 각각의 변수니까 공유하지 않는다 따로 락걸필요가 없다.

#define LOAD_FROM_XML(GameDataName)																								\
			case GameDataType::GameDataName:																					\
				{																												\
					const GameDataName##Key key(FromStringCast<GameDataName##KeyType>(*keyString));								\
					GameDataName* gameData = new GameDataName;																	\
					GameDataName##LoadHelper* gameDataLoadHelper = new GameDataName##LoadHelper(this);							\
					gameData->clear();																							\
					gameDataLoadHelper->clear();																				\
																																\
					gameData->_key = key;																						\
					gameDataLoadHelper->_key = key;																				\
					if (false == gameData->loadFromXml(childElement, gameDataLoadHelper))										\
					{																											\
						TA_ASSERT_DEV(false, "게임데이터 LoadFromXml에 실패했습니다. type : %d, key : %d"							\
									  , static_cast<uint8>(gameDataType)														\
									  , key.getKeyValue());																		\
																																\
						return false;																							\
					}																											\
					gameData->setIsValid(true);																					\
					gameDataLoadHelpers.push_back(gameDataLoadHelper);															\
																																\
					std::pair<std::unordered_map<GameDataName##Key, GameDataName *>::iterator, bool> rv							\
						= _setOf##GameDataName.insert(std::make_pair(key, gameData));											\
																																\
					if (false == rv.second)																						\
					{																											\
						TA_ASSERT_DEV(false, "Key값 중복됩니다.type : %d, key : %d"												\
									  , static_cast<uint8>(gameDataType)														\
									  , key.getKeyValue());																		\
																																\
						return false;																							\
					}																											\
				}																												\
				break;																											\


				LOAD_FROM_XML(GroupGameData)
					LOAD_FROM_XML(MoveGameData)
					LOAD_FROM_XML(AiGameData)
					LOAD_FROM_XML(CharacterGameData)
					LOAD_FROM_XML(ItemGameData)
					LOAD_FROM_XML(RenderingGameData)
					LOAD_FROM_XML(ConditionGameData)
					LOAD_FROM_XML(EventGameData)
					LOAD_FROM_XML(BuffGameData)

					TA_COMPILE_DEV(9 == static_cast<uint8>(GameDataType::Count), "여기도 확인해주세요")
#undef LOAD_FROM_XML

			default:
				{
					TA_ASSERT_DEV(false, "비정상");
					return false;
				}
				break;
			}
		}
		return true;
	}

	bool GameDataManager::finishGameDataLoading(const GameDataType& gameDataType, const std::vector<GameDataLoadHelper*>& gameDataLoadHelperSet) noexcept
	{
		if (GameDataType::Count <= gameDataType)
		{
			TA_ASSERT_DEV(false, "비정상");
			return false;
		}

		std::wstringstream ss;
		ss << std::this_thread::get_id();
		TA_LOG_DEV("[FinishLoading] => GameDataType : %d, Thread id : %s", static_cast<uint32>(gameDataType), ss.str().c_str());

		const uint32 count = gameDataLoadHelperSet.size();
		for (uint32 index = 0; index < count; ++index)
		{
		
			switch (gameDataType)
			{

#define FINISH_LOADING(GameDataName)																									\
			case GameDataType::GameDataName:																							\
				{																														\
					GameDataName##LoadHelper* currentLoadHelper = static_cast<GameDataName##LoadHelper*>(gameDataLoadHelperSet[index]);	\
					const GameDataName##KeyType currentKeyValue = currentLoadHelper->_key.getKeyValue();								\
					GameDataName* currentData = static_cast<GameDataName*>(getGameData_(currentLoadHelper->_key.getKeyValue()			\
																						 , GameDataName::getGameDataType()));			\
																																		\
					if (false == currentData->finishLoading(currentLoadHelper))															\
					{																													\
						TA_ASSERT_DEV(false, "게임데이터 FinishLoading에 실패했습니다. type : %d, key : %d"								\
									  , static_cast<uint8>(gameDataType)																\
									  , currentKeyValue);																				\
																																		\
						return false;																									\
					}																													\
				}																														\
				break;																													\


					FINISH_LOADING(GroupGameData)
					FINISH_LOADING(MoveGameData)
					FINISH_LOADING(AiGameData)
					FINISH_LOADING(CharacterGameData)
					FINISH_LOADING(ItemGameData)
					FINISH_LOADING(RenderingGameData)
					FINISH_LOADING(ConditionGameData)
					FINISH_LOADING(EventGameData)
					FINISH_LOADING(BuffGameData)

					TA_COMPILE_DEV(9 == static_cast<uint8>(GameDataType::Count), "여기도 확인해주세요")
#undef FINISH_LOADING


			default:
				{
					TA_ASSERT_DEV(false, "비정상");
					return false;
				}
				break;
			}

		}

		return true;
	}

	bool GameDataManager::checkGameDataFinally(const GameDataType& gameDataType) noexcept
	{
		if (GameDataType::Count <= gameDataType)
		{
			TA_ASSERT_DEV(false, "비정상");
			return false;
		}

		std::wstringstream ss;
		ss << std::this_thread::get_id();
		TA_LOG_DEV("[CheckFinally] => GameDataType : %d, Thread id : %s", static_cast<uint32>(gameDataType), ss.str().c_str());

		switch (gameDataType)
		{
#define CHECK_FINALLY(GameDataName)								\
		case GameDataType::GameDataName:						\
			{													\
				auto it = _setOf##GameDataName.begin();			\
				const auto end = _setOf##GameDataName.end();	\
				while (end != it)								\
				{												\
					it->second->checkFinally(this);				\
					++it;										\
				}												\
			}													\
			break;												\


				CHECK_FINALLY(GroupGameData)
				CHECK_FINALLY(MoveGameData)
				CHECK_FINALLY(AiGameData)
				CHECK_FINALLY(CharacterGameData)
				CHECK_FINALLY(ItemGameData)
				CHECK_FINALLY(RenderingGameData)
				CHECK_FINALLY(ConditionGameData)
				CHECK_FINALLY(EventGameData)
				CHECK_FINALLY(BuffGameData)

				TA_COMPILE_DEV(9 == static_cast<uint8>(GameDataType::Count), "여기도 확인해주세요")
#undef CHECK_FINALLY


		default:
				{
					TA_ASSERT_DEV(false, "비정상");
					return false;
				}
				break;
		}
		return true;
	}

	const GameData* GameDataManager::getGameData(const int32 key, const GameDataType& gameDataType) noexcept
	{
		return static_cast<const GameData*>(getGameData_(key, gameDataType));
	}

	GameData* GameDataManager::getGameData_(const int32 key, const GameDataType& gameDataType) noexcept
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
				RETURN_GAMEDATA(RenderingGameData)
				RETURN_GAMEDATA(ConditionGameData)
				RETURN_GAMEDATA(EventGameData)
				RETURN_GAMEDATA(BuffGameData)

				TA_COMPILE_DEV(9 == static_cast<uint8>(GameDataType::Count), "여기도 확인해주세요")
#undef RETURN_GAMEDATA

		default:
				{
					TA_ASSERT_DEV(false, "비정상입니다.")
				}
				break;
		}

		return nullptr;
	}

}


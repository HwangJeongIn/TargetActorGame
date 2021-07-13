#pragma once

#include "Common/CommonBase.h"
#include "Common/Uncopyable.h"
#include "Common/KeyDefinition.h"
#include "Common/GameData.h"
#include <unordered_map>
#include <filesystem>


namespace fs = std::filesystem;

#ifndef TA_SERVER
namespace ta
{
	extern fs::path GameDataXmlFilePath;
}
#endif

namespace ta
{
	class XmlNode;
	class GameDataLoadHelper;
}


namespace ta
{
	class GameDataManager : public Uncopyable
	{
	public:
		GameDataManager(void) noexcept;
		virtual ~GameDataManager(void) noexcept;

		bool initialize(void) noexcept;
		bool open(void) noexcept;
		void close(void) noexcept;

		bool loadGameDataFromXml(const fs::path filePath, std::vector<std::vector<GameDataLoadHelper*>>* gameDataLoadHelperSet) noexcept;
		bool finishGameDataLoading(const GameDataType& gameDataType, const std::vector<GameDataLoadHelper*>& gameDataLoadHelperSet) noexcept;
		bool checkGameDataFinally(const GameDataType& gameDataType) noexcept;

		const GameData* getGameData(const int32 key, const GameDataType& gameDataType) noexcept;

	private:
		GameData* getGameData_(const int32 key, const GameDataType& gameDataType) noexcept;

	private:

#define DECLARE_GAMEDATA(GameDataName)												  \
		std::unordered_map<GameDataName##Key, GameDataName *> _setOf##GameDataName;   \


		DECLARE_GAMEDATA(GroupGameData)
		DECLARE_GAMEDATA(MoveGameData)
		DECLARE_GAMEDATA(AiGameData)
		DECLARE_GAMEDATA(CharacterGameData)
		DECLARE_GAMEDATA(ItemGameData)
		DECLARE_GAMEDATA(RenderingGameData)
		DECLARE_GAMEDATA(ConditionGameData)
		DECLARE_GAMEDATA(EventGameData)
		DECLARE_GAMEDATA(SectorZoneGameData)


		TA_COMPILE_DEV(9 == static_cast<uint8>(GameDataType::Count), "여기도 확인해주세요")

#undef DECLARE_GAMEDATA
	};
}
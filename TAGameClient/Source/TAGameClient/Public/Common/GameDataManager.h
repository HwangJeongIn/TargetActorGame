#pragma once

#include "Common/CommonBase.h"
#include "Common/Uncopyable.h"
#include "Common/KeyDefinition.h"
#include "Common/GameData.h"
#include <unordered_map>
#include <filesystem>


namespace fs = std::filesystem;


namespace ta
{
	class XmlNode;
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

		const GameData* getGameData(const int32 key, const GameDataType& gameDataType) const noexcept;

		static bool loadGameDataFromXml(GameDataManager* gameDataManager, const fs::path filePath) noexcept;
		//bool makeDataSetWithXmlNode(const std::string)

	private:

#define DECLARE_GAMEDATA(GameDataName)												  \
		std::unordered_map<GameDataName##Key, GameDataName *> _setOf##GameDataName;   \


		DECLARE_GAMEDATA(GroupGameData)
		DECLARE_GAMEDATA(MoveGameData)
		DECLARE_GAMEDATA(AiGameData)
		DECLARE_GAMEDATA(CharacterGameData)
		DECLARE_GAMEDATA(ItemGameData)

		TA_COMPILE_DEV(5 == static_cast<uint8>(GameDataType::Count), "여기도 확인해주세요")

#undef DECLARE_GAMEDATA
	};
}
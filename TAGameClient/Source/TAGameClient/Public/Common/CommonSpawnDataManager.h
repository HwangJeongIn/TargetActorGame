#pragma once
#include "Common/CommonBase.h"
#include "Common/Uncopyable.h"
#include "Common/KeyDefinition.h"
#include <unordered_map>
#include <filesystem>


namespace fs = std::filesystem;

#ifndef TA_SERVER
namespace ta
{
	extern fs::path SpawnDataFilePath;
}
#endif

namespace ta
{
	class CommonActorDetailSpawnData;
}


namespace ta
{
	// 일단 서버만 필요할것같긴한데, 클라에서 데코용 액터 스폰할때 사용할수있으니 일단 Common으로하자
	class CommonSpawnDataManager : public Uncopyable
	{
	public:
		CommonSpawnDataManager(void) noexcept;
		virtual ~CommonSpawnDataManager(void) noexcept;

		virtual bool initialize(void) noexcept;
		virtual bool open(void) noexcept;
		virtual void close(void) noexcept;

		bool loadSpawnDataFromXml(const fs::path filePath) noexcept;


	protected:
		std::unordered_map<GameWorldType, std::vector<CommonActorDetailSpawnData*>> _spawnDataSet;

	};
}

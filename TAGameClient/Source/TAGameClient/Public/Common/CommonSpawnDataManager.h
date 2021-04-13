#pragma once
#include "Common/CommonBase.h"
#include "Common/Uncopyable.h"
#include "Common/KeyDefinition.h"
#include <filesystem>


namespace fs = std::filesystem;


namespace ta
{
#if defined(TA_SERVER) && !defined(TA_CLIENT_IN_SERVER)
	extern const fs::path SpawnDataFilePath;
#elif !defined(TA_SERVER)
	extern fs::path SpawnDataFilePath;
#endif
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

		virtual bool loadSpawnDataFromXml(const fs::path& filePath) noexcept;

	};
}

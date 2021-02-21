#pragma once
#include "Common/CommonBase.h"
#include "Common/CommonSpawnDataManager.h"


namespace ta
{
	class ServerSpawnDataManager : public CommonSpawnDataManager
	{
	public:
		ServerSpawnDataManager(void) noexcept;
		virtual ~ServerSpawnDataManager(void) noexcept;

		virtual bool initialize(void) noexcept override final;
		virtual bool open(void) noexcept override final;
		virtual void close(void) noexcept override final;

		bool spawnWorldActors(const GameWorldType& worldType) noexcept;
		bool spawnRealWorldActors(void) noexcept;

	};
}

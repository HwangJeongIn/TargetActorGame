#pragma once
#include "Common/CommonBase.h"
#include "Common/CommonSpawnDataManager.h"
#include "Common/KeyDefinition.h"
#include "Common/Lockable.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>


namespace ta
{
	typedef std::unordered_map<SpawnDataGroupKey, std::vector<CommonActorDetailSpawnData*>> SpawnDataGroupSet;

	class ServerSpawnDataManager : public CommonSpawnDataManager
	{
	public:
		ServerSpawnDataManager(void) noexcept;
		virtual ~ServerSpawnDataManager(void) noexcept;

		virtual bool initialize(void) noexcept override final;
		virtual bool open(void) noexcept override final;
		virtual void close(void) noexcept override final;

		virtual bool loadSpawnDataFromXml(const fs::path& filePath) noexcept override final;

		bool spawnWorldActors(const GameWorldType& worldType) noexcept;
		bool spawnRealWorldActors(void) noexcept;

	private:
		bool loadSpawnDataGroupConfig(const fs::path& filePath) noexcept;


	private:
		LockableObject _spawnDataGroupWorldSetLoadLock;
		std::unordered_set<SpawnDataGroupKey> _realWorldSpawnGroupList; // 초반에 판별할때만 사용할듯
		std::unordered_map<GameWorldType, SpawnDataGroupSet> _spawnDataGroupWorldSet;
	};
}

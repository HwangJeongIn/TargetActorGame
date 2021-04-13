#pragma once

#include "Common/CommonBase.h"
#include "Common/CommonActorManager.h"


namespace ta
{
	class CharacterDBNo;
	class ActorKey;
	class CommonActorDetailSpawnDataForCharacter;
	class CommonActorDetailSpawnData;
}


namespace ta
{
	class ServerActorManager : public CommonActorManager
	{
		friend class ServerApp; // 여기서만 만들어야해서 프렌드 선언
	public:
		virtual bool initialize(void) noexcept override final;
		virtual bool open(void) noexcept override final;
		virtual void close(void) noexcept override final;

		virtual Sector* getSector(const SectorKey& sectorKey) noexcept override final;

		//CommonActor* createActorAndInitializeFromDB(const ActorType& actorType, const CharacterDBNo& characterNo) noexcept;
		CommonActor* createCharacterActorAndInitializeComponentsFromSpawnData(const CommonActorDetailSpawnDataForCharacter& detailSpawnData) noexcept;

		// 플레이어용도 // 한번에 처리못해서 분리했음
		CommonActor* createActorForServer(const ActorType& actorType) noexcept;
		bool initializeActorComponentsFromDB(const ActorKey& targetActorKey, const CharacterDBNo& characterDBNo) noexcept;
	private:
		//CommonActor* createDefaultActorForServer(void) noexcept;
		// 플레이어
		CommonActor* createPlayerActorForServer(void) noexcept; // 클라에서 요청들어오면 거기에 맞춰 생성된다.
		// 몬스터
		//CommonActor* createMonsterActorForServer(void) noexcept;
		CommonActor* createNpcActorForServer(void) noexcept;

		bool initializeActorComponentsFromData(const ActorKey& targetActorKey, const CommonActorDetailSpawnData& detailSpawnData) noexcept;

	private:
		ServerActorManager(void) noexcept;
		virtual ~ServerActorManager(void) noexcept;
	};
}

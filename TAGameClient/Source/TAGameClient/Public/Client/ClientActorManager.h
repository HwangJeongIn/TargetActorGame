#pragma once

#include "Common/CommonBase.h"
#include "Common/CommonActorManager.h"
#include "Common/KeyDefinition.h"


namespace ta
{
	class ClientActorManager : public CommonActorManager
	{
		friend class ClientApp; // 여기서만 만들어야해서 프렌드 선언
	public:
		virtual bool initialize(void) noexcept override final;
		virtual bool open(void) noexcept override final;
		virtual void close(void) noexcept override final;

		virtual Sector* getSector(const SectorKey& sectorKey) noexcept override final;

		// 클라에서 여러캐릭터 같은소켓 공유하려고 만든 함수
		CommonActor* createActorForClient(const ActorType& actorType, const ActorKey& actorKeyReceived) noexcept;

		const ActorKey getMainActorKeyWithCompletionKey(const ActorKey& completionKey) noexcept;
		bool bindActorAsMainPlayerActor(const ActorKey& targetActorKey) noexcept;
		
		CommonActor* getMainPlayerActor(void) noexcept;
	private:
		ClientActorManager(void) noexcept;
		virtual ~ClientActorManager(void) noexcept;

		CommonActor* createPlayerActorForClient(const ActorKey& actorKeyReceived) noexcept;
		CommonActor* createControlledByPlayerActorForClient(const ActorKey& actorKeyReceived) noexcept;
		CommonActor* createNpcActorForClient(const ActorKey& actorKeyReceived) noexcept;
		CommonActor* createObjectActorForClient(const ActorKey& actorKeyReceived) noexcept;


	private:
		// Client Owner(액터키 0번)에 연결된 메인액터키
		ActorKey _mainPlayerActorKey;
	};

}

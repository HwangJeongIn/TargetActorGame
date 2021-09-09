#pragma once

#include "Common/Sector.h"
#include "Common/KeyDefinition.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>


namespace ta
{
	class SectorProcessor;
	class SectorEventSet;
	class ContentParameter;
}


namespace ta
{
	class ServerSector : public Sector
	{
	public:
		ServerSector(void) noexcept;
		virtual ~ServerSector(void) noexcept override final;

		virtual uint32 getOwnedActorCountForServer_(const GroupGameDataKey& groupGameDataKey) const noexcept override final;
		virtual bool checkOwnedActorExistenceForServer_(const GroupGameDataKey& groupGameDataKey) const noexcept override final;


		virtual bool registerToOwnedActorsForServer_(const GroupGameDataKey& groupGameDataKey, const ActorKey& actorKey) noexcept override final;
		virtual bool deregisterFromOwnedActorsForServer_(const GroupGameDataKey& groupGameDataKey, const ActorKey& actorKey) noexcept override final;
		
		bool initializeSectorEvents(SectorEventSet* sectorEvents) noexcept;

		bool startSectorEvents(const ContentParameter& parameter) noexcept;

		virtual bool startSectorEventForServer(const ContentParameter& parameter, uint32 sectorEventIndex, bool isBasicSectorEvent) noexcept override final;

	private:
		// 섹터가 스폰한 액터 // 따로 관리하는 이유는 섹터이벤트 처리할 때 컨디션체크를 해야하기 때문
		std::unordered_map<GroupGameDataKey, std::unordered_set<ActorKey>> _ownedActorMap;
		std::unordered_set<ActorKey> _ownedActorSet;

		SectorEventSet* _sectorEvents;
		
		// SectorEvent / SectorProperty 등을 처리
		SectorProcessor* _sectorProcessor;
	};
}
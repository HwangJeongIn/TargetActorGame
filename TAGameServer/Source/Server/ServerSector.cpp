﻿#include "Server/ServerSector.h"
#include "Common/SectorProcessor.h"
#include "Common/SectorEventSet.h"
#include "Common/ContentParameter.h"


namespace ta
{
	ServerSector::ServerSector(void) noexcept
		: _sectorProcessor(new SectorProcessor(this))
		, _sectorEvents(nullptr)
	{
	}

	ServerSector::~ServerSector(void) noexcept
	{
		if (nullptr != _sectorProcessor)
		{
			delete _sectorProcessor;
		}

		if (nullptr != _sectorEvents)
		{
			delete _sectorEvents;
		}
	}

	uint32 ServerSector::getOwnedActorCountForServer_(const GroupGameDataKey& groupGameDataKey) const noexcept
	{
		auto rv = _ownedActorMap.find(groupGameDataKey);
		if (_ownedActorMap.end() == rv)
		{
			return 0;
		}

		return rv->second.size();
	}

	bool ServerSector::checkOwnedActorExistenceForServer_(const GroupGameDataKey& groupGameDataKey) const noexcept
	{
		return 0 == getOwnedActorCountForServer_(groupGameDataKey) ? false : true;
	}

	bool ServerSector::registerToOwnedActorsForServer_(const GroupGameDataKey& groupGameDataKey, const ActorKey& actorKey) noexcept
	{
		{
			auto rv = _ownedActorSet.insert(actorKey);
			if (false == rv.second)
			{
				TA_ASSERT_DEV(false, "비정상");
				return false;
			}
		}

		{
			auto rv = _ownedActorMap.insert(std::pair<GroupGameDataKey, std::unordered_set<ActorKey>>(groupGameDataKey
																							   , std::unordered_set<ActorKey>()));
			auto rv2 = rv.first->second.insert(actorKey);
			if (false == rv2.second)
			{
				TA_ASSERT_DEV(false, "비정상");
				return false;
			}
		}

		return true;
	}

	bool ServerSector::deregisterFromOwnedActorsForServer_(const GroupGameDataKey& groupGameDataKey, const ActorKey& actorKey) noexcept
	{
		{
			size_t rv = _ownedActorSet.erase(actorKey);
			if (1 != rv)
			{
				TA_ASSERT_DEV(false, "비정상");
				return false;
			}
		}

		{
			auto rv = _ownedActorMap.find(groupGameDataKey);
			if (_ownedActorMap.end() == rv)
			{
				TA_ASSERT_DEV(false, "비정상");
				return false;
			}
				
			size_t rv2 = rv->second.erase(actorKey);
			if (1 != rv2)
			{
				TA_ASSERT_DEV(false, "비정상");
				return false;
			}
		}

		return true;
	}
	
	bool ServerSector::initializeSectorEvents(SectorEventSet* sectorEvents) noexcept
	{
		if (nullptr != _sectorEvents)
		{
			TA_ASSERT_DEV(false, "이미 초기화 되었습니다.");
			return false;
		}

		if (nullptr == sectorEvents)
		{
			TA_ASSERT_DEV(false, "비정상");
			return false;
		}

		_sectorEvents = sectorEvents;

		return true;
	}

	bool ServerSector::startSectorEvents(const ContentParameter& parameter) noexcept
	{
		return _sectorEvents->startBasicSectorEvents(parameter);
	}
	
	bool ServerSector::startSectorEventForServer(const ContentParameter& parameter, uint32 sectorEventIndex, bool isBasicSectorEvent) noexcept
	{
		return _sectorEvents->startSectorEvent(parameter, sectorEventIndex, isBasicSectorEvent);
	}
}

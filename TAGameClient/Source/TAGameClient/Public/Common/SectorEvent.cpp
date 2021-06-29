#include "Common/SectorEvent.h"
#include "Common/SectorProcessor.h"
#include "Common/GameData.h"
#include "Common/GetComponentAndSystem.h"


namespace ta
{
	SectorEventData::SectorEventData(const SectorEventType& sectorEventType, const float intervalMilliSec) noexcept
		: _sectorEventType(sectorEventType)
		, _intervalMilliSec(intervalMilliSec)
	{
	}

	SectorEventData::~SectorEventData(void) noexcept
	{
	}

	const SectorEventType SectorEventData::getSectorEventType(void) const noexcept
	{
		return _sectorEventType;
	}
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	SectorEvent::SectorEvent(void) noexcept
	{
	}

	SectorEvent::~SectorEvent(void) noexcept
	{
	}
	
	bool SectorEvent::initializeSectorEvent(const SectorEventData* sectorEventData) noexcept
	{
		clear();

		if (nullptr == sectorEventData)
		{
			TA_ASSERT_DEV(false, "비정상");
			return false;
		}

		if (false == checkProperData(sectorEventData))
		{
			TA_ASSERT_DEV(false, "비정상");
			return false;
		}

		_sectorEventData = sectorEventData;

		return true;
	}

	void SectorEvent::clear(void) noexcept
	{
		_sectorEventData = nullptr;
	}

}


namespace ta
{

	SectorEventDataSpawnNpc::SectorEventDataSpawnNpc(const float intervalMilliSec) noexcept
		: SectorEventData(SectorEventType::SectorEventSpawnNpc, intervalMilliSec)
	{
	}

	SectorEventDataSpawnNpc::~SectorEventDataSpawnNpc(void) noexcept
	{
	}

	const GroupGameData* SectorEventDataSpawnNpc::getNpcGroupGameData(void) const noexcept
	{
		return _npcGroupGameData;
	}

	bool SectorEventDataSpawnNpc::setNpcGroupGameData(const GroupGameDataKey& npcGroupGameDataKey) noexcept
	{
		if (false == npcGroupGameDataKey.isValid())
		{
			TA_ASSERT_DEV(false, "비정상");
			return false;
		}

		_npcGroupGameData = GetGameData<GroupGameData>(npcGroupGameDataKey);
		if (nullptr == _npcGroupGameData)
		{
			TA_ASSERT_DEV(false, "비정상");
			return false;
		}
		return true;
	}

	const uint8 SectorEventDataSpawnNpc::getMaxActorCount(void) const noexcept
	{
		return _maxActorCount;
	}

	bool SectorEventDataSpawnNpc::setMaxActorCount(const uint8 maxActorCount) noexcept
	{
		if (MaxActorCountSpawnedBySectorEvent <= maxActorCount)
		{
			TA_ASSERT_DEV(false, "비정상");
			return false;
		}

		_maxActorCount = maxActorCount;
		return true;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	SectorEventSpawnNpc::SectorEventSpawnNpc(void) noexcept
	{
	}

	SectorEventSpawnNpc::~SectorEventSpawnNpc(void) noexcept
	{
	}

	void SectorEventSpawnNpc::clear(void) noexcept
	{
		SectorEvent::clear();

		_maxActorCount = 0;
		//_npcGroup
	}

	bool SectorEventSpawnNpc::checkProperData(const SectorEventData* sectorEventData) const noexcept
	{
		return false;
	}
}


#include "Common/SectorEventSet.h"
#include "Common/GameData.h"
#include "Common/GetComponentAndSystem.h"
#include "SectorEventSet.h"


namespace ta
{
	SpecialSectorEvent::SpecialSectorEvent(void) noexcept
	{
	}

	SpecialSectorEvent::~SpecialSectorEvent(void) noexcept
	{
	}
	bool SpecialSectorEvent::canOccur(const ContentParameter& parameter) noexcept
	{
		return _conditionGameData->checkCondition(parameter);
	}

	void SpecialSectorEvent::setData(const EventGameData* eventGameData, const ConditionGameData* conditionGameData) noexcept
	{
		_eventGameData = eventGameData;
		_conditionGameData = conditionGameData;
	}
}


namespace ta
{
	SectorEventSetData::SectorEventSetData(void) noexcept
	{
	}

	SectorEventSetData::~SectorEventSetData(void) noexcept
	{
	}

	void SectorEventSetData::makeSetFromSetData(SectorEventSet& sectorEventSet) noexcept
	{
	}
}


namespace ta
{
	SectorEventSet::SectorEventSet(const std::vector<const EventGameData*>& basicEventGameDataSetData
								   , const std::vector<const SpecialSectorEvent*>& specialEventGameDataSetData) noexcept
		: _basicEventGameDataSet(basicEventGameDataSetData)
		, _specialEventGameDataCandidates(specialEventGameDataSetData)
	{
	}

	SectorEventSet::~SectorEventSet(void) noexcept
	{
	}
}
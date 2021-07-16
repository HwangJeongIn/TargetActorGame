#include "Common/SectorEventSet.h"
#include "Common/GameData.h"
#include "Common/GetComponentAndSystem.h"
#include "SectorEventSet.h"


namespace ta
{
	SpecialSectorEvent::SpecialSectorEvent(void) noexcept
	{
	}

	SpecialSectorEvent::SpecialSectorEvent(const EventGameData* eventGameData, const ConditionGameData* conditionGameData) noexcept
		: _eventGameData(eventGameData)
		, _conditionGameData(conditionGameData)
	{
	}

	SpecialSectorEvent::~SpecialSectorEvent(void) noexcept
	{
	}

	bool SpecialSectorEvent::canOccur(const ContentParameter& parameter) const noexcept
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
		const uint32 count = _specialEventGameDataSetData.size();
		for (uint32 index = 0; index < count; ++index)
		{
			delete _specialEventGameDataSetData[index];
		}

		_basicEventGameDataSetData.clear();
		_specialEventGameDataSetData.clear();
	}

	SectorEventSet* SectorEventSetData::makeSetFromSetData(void) noexcept
	{
		SectorEventSet* rv = new SectorEventSet(_basicEventGameDataSetData, _specialEventGameDataSetData);
		return rv;
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

	void SectorEventSet::onChangeSector(const ContentParameter& parameter) noexcept
	{
		const uint32 activatedSpecialEventCount = _activatedSpecialEventGameDataSet.size();
		const uint32 specialEventCandidateCount = _specialEventGameDataCandidates.size();

		// 활성화되어 있던 이벤트 조건 확인
		{
			auto it = _activatedSpecialEventGameDataSet.begin();
			const auto end = _activatedSpecialEventGameDataSet.end();
			while (end != it)
			{
				if (false == (*it)->canOccur(parameter))
				{
					_specialEventGameDataCandidates.push_back((*it));
					it = _activatedSpecialEventGameDataSet.erase(it);
					continue;
				}

				++it;
			}
		}

		// 후보 이벤트 조건 확인
		{
			auto it = _specialEventGameDataCandidates.begin();
			const auto end = _specialEventGameDataCandidates.end();
			uint32 currentCount = 0;
			while (end != it 
				   && specialEventCandidateCount > currentCount)
			{
				++currentCount;
				if (false == (*it)->canOccur(parameter))
				{
					_activatedSpecialEventGameDataSet.push_back((*it));
					it = _specialEventGameDataCandidates.erase(it);
					continue;
				}

				++it;
			}
		}
	}
}
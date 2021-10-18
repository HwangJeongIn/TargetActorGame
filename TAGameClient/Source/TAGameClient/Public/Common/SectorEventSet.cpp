#include "Common/SectorEventSet.h"
#include "Common/GameData.h"
#include "Common/GetComponentAndSystem.h"
#include "Common/ScopedLock.h"
#include "Common/SectorEventSet.h"
#include "Common/ContentEventTimer.h"
#include "Common/ContentEvent.h"
#include "Common/ContentParameter.h"



namespace ta
{
	SpecialSectorEvent::SpecialSectorEvent(const EventGameData* eventGameData, const ConditionGameData* conditionGameData) noexcept
		: _eventGameData(eventGameData)
		, _conditionGameData(conditionGameData)
		, _isActivated(false)
	{
	}

	SpecialSectorEvent::SpecialSectorEvent(const SpecialSectorEvent* specialSectorEvent) noexcept
		: _eventGameData(specialSectorEvent->_eventGameData)
		, _conditionGameData(specialSectorEvent->_conditionGameData)
		, _isActivated(false)
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
	
	bool SpecialSectorEvent::getIsActivated(void) const noexcept
	{
		return _isActivated;
	}
	
	void SpecialSectorEvent::setIsActivated(const bool flag) noexcept
	{
		_isActivated = flag;
	}
	
	bool SpecialSectorEvent::checkAndExecute(const ContentParameter& parameter) const noexcept
	{
		if (false == _isActivated)
		{
			TA_ASSERT_DEV(false, "활성화되어 있지 않습니다.");
			return false;
		}

		if (false == _eventGameData->checkAndExecute(parameter))
		{
			TA_LOG_DEV("조건이 만족되지 않았습니다.");
			return false;
		}

		return true;
	}
	
	uint32 SpecialSectorEvent::getInterval(void) const noexcept
	{
		return _eventGameData->_interval;
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

	SectorEventSet* SectorEventSetData::makeSetFromSetData(void) const noexcept
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
	{
		const uint32 count = specialEventGameDataSetData.size();
		_specialEventGameDataSet.reserve(count);
		for (uint32 index = 0; index < count; ++index)
		{
			SpecialSectorEvent* newSpecialSectorEvent = new SpecialSectorEvent(specialEventGameDataSetData[index]);
			_specialEventGameDataSet.push_back(newSpecialSectorEvent);
		}
	}

	SectorEventSet::~SectorEventSet(void) noexcept
	{
	}

	void SectorEventSet::onChangeSector(const ContentParameter& parameter) noexcept
	{
		const uint32 specialEventCount = _specialEventGameDataSet.size();
		
		std::vector<uint32> newSpecialEvents;
		// 활성화되어 있던 이벤트 조건 확인
		{
			ScopedLock eventLock(this);

			for (uint32 index = 0; index < specialEventCount; ++index)
			{
				const bool isActivated = _specialEventGameDataSet[index]->getIsActivated();

				// 현재와 반대의 상황이나오면 뒤집어 줘야 한다.
				if (isActivated != _specialEventGameDataSet[index]->canOccur(parameter))
				{
					_specialEventGameDataSet[index]->setIsActivated(!isActivated);
					
					// 새롭게 활성화된 경우는 아래에서 이벤트를 수행해야한다. // 락 안걸고 사용하기 위해서 아래에서 처리
					if (false == isActivated)
					{
						newSpecialEvents.push_back(index);
					}
				}
			}
		}

		{
			const uint32 count = newSpecialEvents.size();
			for (uint32 index = 0; index < count; ++index)
			{
				if (false == startSectorEvent(parameter, newSpecialEvents[index], false))
				{
					TA_ASSERT_DEV(false, "비정상입니다.");
					// return;
				}
			}
		}

	}
	
	bool SectorEventSet::startBasicSectorEvents(const ContentParameter& parameter) noexcept
	{
		const uint32 count = _basicEventGameDataSet.size();
		
		bool rv = true;
		for (uint32 index = 0; index < count; ++index)
		{
			if (false == startSectorEvent(parameter, index, true))
			{
				TA_ASSERT_DEV(false, "비정상입니다.");
				return false;
			}
		}

		return rv;
	}

	bool SectorEventSet::startSectorEvent(const ContentParameter& parameter, uint32 sectorEventIndex, bool isBasicSectorEvent) noexcept
	{
		uint32 interval = 0;
		//bool registerNewEvent = false;

		if (true == isBasicSectorEvent)
		{
			const uint32 count = _basicEventGameDataSet.size();
			if (count <= sectorEventIndex)
			{
				TA_ASSERT_DEV(false, "이벤트 인덱스가 비정상입니다.");
				return false;
			}

			if (false == _basicEventGameDataSet[sectorEventIndex]->checkAndExecute(parameter))
			{
				TA_ASSERT_DEV(false, "비정상입니다.");
				return false;
			}

			interval = _basicEventGameDataSet[sectorEventIndex]->_interval;

		}
		else
		{
			const uint32 count = _specialEventGameDataSet.size();
			if (count <= sectorEventIndex)
			{
				TA_ASSERT_DEV(false, "이벤트 인덱스가 비정상입니다.");
				return false;
			}

			if (false == _specialEventGameDataSet[sectorEventIndex]->checkAndExecute(parameter))
			{
				TA_ASSERT_DEV(false, "비정상입니다.");
				return false;
			}

			interval = _specialEventGameDataSet[sectorEventIndex]->getInterval();
		}

		ContentEventSectorObject* sectorEvent = new ContentEventSectorObject;
		sectorEvent->_contentEventType = ContentEventType::SectorEvent;
		sectorEvent->_sectorKey = parameter._sectorKey;
		sectorEvent->_sectorEventIndex = sectorEventIndex;
		sectorEvent->_isBasicSectorEvent = isBasicSectorEvent;

		TA_LOG_DEV("섹터 이벤트 다시 등록 SectorKey : %d, SectorEventIndex : %d, IsBasicSectorEvent : %d"
				   , sectorEvent->_sectorKey.getKeyValue()
				   , sectorEvent->_sectorEventIndex
				   , sectorEvent->_isBasicSectorEvent);

		if (false == RegisterContentEvent(sectorEvent, interval))
		{
			TA_ASSERT_DEV(false, "이벤트 등록에 실패했습니다.");
			return false;
		}

		return true;
	}
}
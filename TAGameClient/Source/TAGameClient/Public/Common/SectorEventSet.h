#pragma once
#include "Common/CommonBase.h"
#include "Common/KeyDefinition.h"

namespace ta
{
	class EventGameData;
	class ConditionGameData;
	class ContentParameter;
	class SectorEventSet;
}


namespace ta
{
	class SpecialSectorEvent
	{
	public:
		SpecialSectorEvent(void) noexcept;
		virtual ~SpecialSectorEvent(void) noexcept;

		bool canOccur(const ContentParameter& parameter) noexcept;
		void setData(const EventGameData* eventGameData, const ConditionGameData* conditionGameData) noexcept;
	
	private:
		const EventGameData* _eventGameData;
		const ConditionGameData* _conditionGameData;
	};
}


namespace ta
{
	class SectorEventSetData
	{
	public:
		SectorEventSetData(void) noexcept;
		virtual ~SectorEventSetData(void) noexcept;

		void makeSetFromSetData(SectorEventSet& sectorEventSet) noexcept;

	public:
		std::vector<const EventGameData*> _basicEventGameDataSetData;
		std::vector<const SpecialSectorEvent*> _specialEventGameDataSetData;

	};
}


namespace ta
{
	class SectorEventSet
	{
	public:
		explicit SectorEventSet(const std::vector<const EventGameData*>& basicEventGameDataSetData
								, const std::vector<const SpecialSectorEvent*>& specialEventGameDataSetData) noexcept;

		virtual ~SectorEventSet(void) noexcept;

	public:
		const std::vector<const EventGameData*> _basicEventGameDataSet;
		std::vector<const SpecialSectorEvent*> _activatedSpecialEventGameDataSet;
		std::vector<const SpecialSectorEvent*> _specialEventGameDataCandidates;

	};
}

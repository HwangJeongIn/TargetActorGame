#pragma once
#include "Common/CommonBase.h"
#include "Common/KeyDefinition.h"
#include "Common/Lockable.h"


namespace ta
{
	class EventGameData;
	class ConditionGameData;
	class ContentParameter;
	class SectorEventSet;
	class ContentParameter;
}


namespace ta
{
	class SpecialSectorEvent
	{
	public:
		explicit SpecialSectorEvent(const EventGameData* eventGameData, const ConditionGameData* conditionGameData) noexcept;
		explicit SpecialSectorEvent(const SpecialSectorEvent* specialSectorEvent) noexcept;
		virtual ~SpecialSectorEvent(void) noexcept;

		bool canOccur(const ContentParameter& parameter) const noexcept;
		void setData(const EventGameData* eventGameData, const ConditionGameData* conditionGameData) noexcept;

		bool getIsActivated(void) const noexcept;
		void setIsActivated(const bool flag) noexcept;
	
		bool checkAndExecute(const ContentParameter& parameter) const noexcept;
		uint32 getInterval(void) const noexcept;
	private:
		const EventGameData* _eventGameData;
		const ConditionGameData* _conditionGameData;
		bool _isActivated;
	};
}


namespace ta
{
	class SectorEventSetData
	{
	public:
		SectorEventSetData(void) noexcept;
		virtual ~SectorEventSetData(void) noexcept;

		SectorEventSet* makeSetFromSetData(void) const noexcept;

	public:
		std::vector<const EventGameData*> _basicEventGameDataSetData;
		std::vector<const SpecialSectorEvent*> _specialEventGameDataSetData;

	};
}


namespace ta
{
	class SectorEventSet : public Lockable
	{
	public:
		explicit SectorEventSet(const std::vector<const EventGameData*>& basicEventGameDataSetData
								, const std::vector<const SpecialSectorEvent*>& specialEventGameDataSetData) noexcept;

		virtual ~SectorEventSet(void) noexcept;

		void onChangeSector(const ContentParameter& parameter) noexcept;
		bool startBasicSectorEvents(const ContentParameter& parameter) noexcept;
		bool startSectorEvent(const ContentParameter& parameter, uint32 eventIndex, bool isBasic = true) noexcept;

	public:
		const std::vector<const EventGameData*> _basicEventGameDataSet;
		std::vector<SpecialSectorEvent*> _specialEventGameDataSet;

	};
}

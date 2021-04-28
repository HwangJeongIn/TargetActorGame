#pragma once
#include "Common/CommonBase.h"
#include "Common/KeyDefinition.h"


/*
EventData : 고정적인 데이터만 가지고 있다. 기능을 가지지 않는다. 데이터의 역할만 한다.
Event : 이벤트 발생에 따라서 변하는, 가변적인 변수를 가지고 있으며, 객체 타입에 따라서 기능을 다양하게 가진다.

EventData는 처음 시작할때 파일에서 로드되어 불변하기 때문에 공통적으로 사용하고, Event는 사용하는 쪽에서 각각 관리한다.
*/


namespace ta
{
	TA_COMPILE_DEV(2 == static_cast<uint8>(SectorEventType::Count), "여기도 확인해주세요");

	class SectorProcessor;
	class GroupGameData;
}


namespace ta
{
	class SectorEventData
	{
	public:
		explicit SectorEventData(const SectorEventType& sectorEventType, const float intervalMilliSec) noexcept;
		virtual ~SectorEventData(void) noexcept;

		const SectorEventType getSectorEventType(void) const noexcept;

	protected:
		const float _intervalMilliSec;
		const SectorEventType _sectorEventType;
	};


	class SectorEvent
	{
	public:
		SectorEvent(void) noexcept;
		virtual ~SectorEvent(void) noexcept;

		// 생성자에서 처리안하고 따로 단계로 나눈이유는 생성자에서 처리할경우 리턴처리가 불가능해서 외부에서 알 수 없고, 
		// 각 객체마다 제대로된 데이터가 들어왔는지 체크하는 checkProperData함수가 가상함수기 때문에 생성자에서 사용하면 안된다.
		bool initializeSectorEvent(const SectorEventData* sectorEventData) noexcept;

		virtual void clear(void) noexcept;
		virtual bool checkProperData(const SectorEventData* sectorEventData) const noexcept = 0;

		virtual bool checkCondition(const SectorProcessor& sectorProcessor) const noexcept = 0;
		virtual void processEvent(const SectorProcessor& sectorProcessor) const noexcept = 0;

	protected:
		const SectorEventData* _sectorEventData;
	};
}


namespace ta
{
	class SectorEventDataSpawnNpc : public SectorEventData
	{
	public:
		explicit SectorEventDataSpawnNpc(const float intervalMilliSec) noexcept;
		
		virtual ~SectorEventDataSpawnNpc(void) noexcept;

		const GroupGameData* getNpcGroupGameData(void) const noexcept;
		bool setNpcGroupGameData(const GroupGameDataKey& npcGroupGameDataKey) noexcept;
		
		const uint8 getMaxActorCount(void) const noexcept;
		bool setMaxActorCount(const uint8 maxActorCount) noexcept;

	private:
		const GroupGameData* _npcGroupGameData;
		uint8 _maxActorCount;
	};


	class SectorEventSpawnNpc : public SectorEvent
	{
	public:
		SectorEventSpawnNpc(void) noexcept;
		virtual ~SectorEventSpawnNpc(void) noexcept;

		virtual void clear(void) noexcept override final;
		virtual bool checkProperData(const SectorEventData* sectorEventData) const noexcept override final;

	private:
		GroupGameDataKey _targetGroupGameDataKey;
		uint8 _maxActorCount;

	};
}

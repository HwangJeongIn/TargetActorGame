#pragma once

#include "Common/CommonBase.h"
#include "Common/KeyDefinition.h"
#include "Common/Socket.h"
#include <chrono>


namespace ta
{
	class ContentEventObject
	{
	public:
		ContentEventObject(void) noexcept;
		~ContentEventObject(void) noexcept;

		void clear(void) noexcept;

		constexpr bool operator<(const ContentEventObject& _left) const noexcept;
		void setWakeupTime(const long long delayMilliSec) noexcept;

	public:
		ContentEventType _contentEventType;
		AiCommandType _aiCommandType; // ActorEventType::AiEvent일때만 유효

		ActorKey _myActorKey;
		ActorKey _targetActorKey;

		// 섹터 관련 이벤트에서 사용
		SectorKey _sectorKey;
		bool _sectorEventIndex;
		bool _isBasicSectorEvent;


		// 자동으로 채워진다.
	public:
		std::chrono::high_resolution_clock::time_point _wakeupTime;
		float _deltaTime;
	};
}


namespace ta
{
	class ContentEventOverlappedStruct : public OverlappedStructBase
	{
	public:
		ContentEventOverlappedStruct(void) noexcept;
		virtual ~ContentEventOverlappedStruct(void) noexcept;

		void clear(void) noexcept;

	public:
		ContentEventObject* _contentEventObject;
	};
}
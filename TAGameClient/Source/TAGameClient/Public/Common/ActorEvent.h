#pragma once

#include "Common/CommonBase.h"
#include "Common/KeyDefinition.h"
#include "Common/Socket.h"
#include <chrono>


namespace ta
{
	class ActorEventObject
	{
	public:
		ActorEventObject(void) noexcept;
		~ActorEventObject(void) noexcept;

		void clear(void) noexcept;

		constexpr bool operator<(const ActorEventObject& _left) const noexcept;
		void setWakeupTime(const long long delayMilliSec) noexcept;

	public:
		ActorEventType _actorEventType;
		AiCommandType _aiCommandType; // ActorEventType::AiEvent일때만 유효

		ActorKey _myActorKey;
		ActorKey _targetActorKey;

		// 자동으로 채워진다.
	public:
		std::chrono::high_resolution_clock::time_point _wakeupTime;
		float _deltaTime;
	};
}


namespace ta
{
	class ActorEventOverlappedStruct : public OverlappedStructBase
	{
	public:
		ActorEventOverlappedStruct(void) noexcept;
		virtual ~ActorEventOverlappedStruct(void) noexcept;

		void clear(void) noexcept;

	public:
		ActorEventObject* _actorEventObject;
	};
}
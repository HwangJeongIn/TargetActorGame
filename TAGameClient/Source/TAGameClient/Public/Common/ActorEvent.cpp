#include "Common/ActorEvent.h"



namespace ta
{
	ActorEventObject::ActorEventObject(void) noexcept
	{
		clear();
	}

	ActorEventObject::~ActorEventObject(void) noexcept
	{
	}

	void ActorEventObject::clear(void) noexcept
	{
		_wakeupTime = {};
		_actorEventType = ActorEventType::Count;
		_aiCommandType = AiCommandType::Count;
		_deltaTime = 0.0f;
	}

	constexpr bool ActorEventObject::operator<(const ActorEventObject& _left) const noexcept
	{
		return (_wakeupTime > _left._wakeupTime);
	}

	void ActorEventObject::setWakeupTime(const long long delayMilliSec) noexcept
	{
		_wakeupTime = std::chrono::high_resolution_clock::now();
		std::chrono::milliseconds delayTime(delayMilliSec);
		_wakeupTime += delayTime;
		_deltaTime = ((float)delayMilliSec / 1000.0f);
	}
}


namespace ta
{
	ActorEventOverlappedStruct::ActorEventOverlappedStruct(void) noexcept
	{
		clear();
	}

	ActorEventOverlappedStruct::~ActorEventOverlappedStruct(void) noexcept
	{
		clear();
	}

	void ActorEventOverlappedStruct::clear(void) noexcept
	{
		OverlappedStructBase::clearBase();
		if (nullptr != _actorEventObject)
		{
			delete _actorEventObject;
		}

		_actorEventObject = nullptr;
	}
}



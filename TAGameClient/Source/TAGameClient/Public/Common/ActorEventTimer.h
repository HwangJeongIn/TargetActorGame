#pragma once

#include "Common/CommonBase.h"
#include "Common/KeyDefinition.h"
#include "Common/ClassProperty.h"
#include <queue>


namespace ta
{
	class ActorEventTimer;
	class Iocp;
	class ActorEventObject;
}


namespace ta
{
	extern void ActorEventTimerThread(ActorEventTimer* actorEventTimer) noexcept;
}


namespace ta
{
	class ActorEventTimer : public Uncopyable, public Lockable
	{
	public:
		explicit ActorEventTimer(Iocp* iocp) noexcept;
		~ActorEventTimer(void) noexcept;

		bool initialize(void) noexcept;
		bool open(void) noexcept;
		void close(void) noexcept;

		bool addEvent(ActorEventObject* actorEventObject, const long long delayMilliSec) noexcept;
		bool canWakeup_(ActorEventType& output) noexcept;
		bool popAndProcessEvent_(void) noexcept;

	private:
		std::priority_queue<ActorEventObject*> _actorEventQueue;
		Iocp* _iocp;
	};
}
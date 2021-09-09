#pragma once

#include "Common/CommonBase.h"
#include "Common/KeyDefinition.h"
#include "Common/ClassProperty.h"
#include <queue>


namespace ta
{
	class ContentEventTimer;
	class Iocp;
	class ContentEventObject;
}


namespace ta
{
	extern void ContentEventTimerThread(ContentEventTimer* actorEventTimer) noexcept;
}


namespace ta
{
	class ContentEventTimer : public Uncopyable, public Lockable
	{
	public:
		explicit ContentEventTimer(Iocp* iocp) noexcept;
		~ContentEventTimer(void) noexcept;

		bool initialize(void) noexcept;
		bool open(void) noexcept;
		void close(void) noexcept;

		bool addEvent(ContentEventObject* actorEventObject, const long long delayMilliSec) noexcept;
		bool canWakeup_(ContentEventType& output) noexcept;
		bool popAndProcessEvent_(void) noexcept;

	private:
		std::priority_queue<ContentEventObject*> _contentEventQueue;
		Iocp* _iocp;
	};
}
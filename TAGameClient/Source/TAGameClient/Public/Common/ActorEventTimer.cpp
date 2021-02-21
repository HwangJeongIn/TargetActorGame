#include "Common/ActorEventTimer.h"
#include "Common/ScopedLock.h"
#include "Common/Iocp.h"
#include "Common/ActorEvent.h"


namespace ta
{
	void ActorEventTimerThread(ActorEventTimer* actorEventTimer) noexcept
	{
		ActorEventType actorEventType = ActorEventType::Count;
		while (true)
		{
			Sleep(1);

			while (true)
			{

				ScopedLock lock(actorEventTimer);
				if (false == actorEventTimer->canWakeup_(actorEventType))
				{
					break;
				}

				if (ActorEventType::ThreadEnd == actorEventType)
				{
					return;
				}

				actorEventTimer->popAndProcessEvent_();
			}

		}
	}
}


namespace ta
{
	ActorEventTimer::ActorEventTimer(Iocp* iocp) noexcept
		: _iocp(iocp)
	{
	}

	ActorEventTimer::~ActorEventTimer(void) noexcept
	{
	}

	bool ActorEventTimer::initialize(void) noexcept
	{
		return true;
	}

	bool ActorEventTimer::open(void) noexcept
	{
		return true;
	}

	void ActorEventTimer::close(void) noexcept
	{
		while (false == _actorEventQueue.empty())
		{
			delete _actorEventQueue.top();
			_actorEventQueue.pop();
		}
	}

	bool ActorEventTimer::addEvent(ActorEventObject* actorEventObject, const long long delayMilliSec) noexcept
	{
		ScopedLock lock(this);
		actorEventObject->setWakeupTime(delayMilliSec);
		_actorEventQueue.push(actorEventObject);

		return true;
	}

	bool ActorEventTimer::canWakeup_(ActorEventType& output) noexcept
	{
		if (true == _actorEventQueue.empty())
		{
			return false;
		}

		const ActorEventObject* event = _actorEventQueue.top();
		const bool rv = (event->_wakeupTime < std::chrono::high_resolution_clock::now());
		if (true == rv)
		{
			output = event->_actorEventType;
		}

		return  rv;
	}

	bool ActorEventTimer::popAndProcessEvent_(void) noexcept
	{
		ActorEventObject* event = _actorEventQueue.top();
		
		ActorEventOverlappedStruct* overlappedStruct = new ActorEventOverlappedStruct;

		overlappedStruct->_actorEventObject = event;
		overlappedStruct->_workType = ThreadWorkType::ActorEvent;

		PostQueuedCompletionStatus(_iocp->_handle, 1, event->_myActorKey.getKeyValue(), &overlappedStruct->_wsaOverlapped);

		_actorEventQueue.pop();
		return true;
	}
}

#include "Common/ContentEventTimer.h"
#include "Common/ScopedLock.h"
#include "Common/Iocp.h"
#include "Common/ContentEvent.h"


namespace ta
{
	void ContentEventTimerThread(ContentEventTimer* contentEventTimer) noexcept
	{
		ContentEventType actorEventType = ContentEventType::Count;
		while (true)
		{
			Sleep(1);

			while (true)
			{

				ScopedLock lock(contentEventTimer);
				if (false == contentEventTimer->canWakeup_(actorEventType))
				{
					break;
				}

				// 이벤트 처리하려고 봤더니 끝내라고하면 바로 끝낸다.
				if (ContentEventType::ThreadEnd == actorEventType)
				{
					return;
				}

				contentEventTimer->popAndProcessEvent_();
			}

		}
	}
}


namespace ta
{
	ContentEventTimer::ContentEventTimer(Iocp* iocp) noexcept
		: _iocp(iocp)
	{
	}

	ContentEventTimer::~ContentEventTimer(void) noexcept
	{
	}

	bool ContentEventTimer::initialize(void) noexcept
	{
		return true;
	}

	bool ContentEventTimer::open(void) noexcept
	{
		return true;
	}

	void ContentEventTimer::close(void) noexcept
	{
		while (false == _contentEventQueue.empty())
		{
			delete _contentEventQueue.top();
			_contentEventQueue.pop();
		}
	}

	bool ContentEventTimer::addEvent(ContentEventObject* actorEventObject, const long long delayMilliSec) noexcept
	{
		ScopedLock lock(this);
		actorEventObject->setWakeupTime(delayMilliSec);
		_contentEventQueue.push(actorEventObject);

		return true;
	}

	bool ContentEventTimer::canWakeup_(ContentEventType& output) noexcept
	{
		if (true == _contentEventQueue.empty())
		{
			return false;
		}

		const ContentEventObject* event = _contentEventQueue.top();
		const bool rv = (event->_wakeupTime < std::chrono::high_resolution_clock::now());
		if (true == rv)
		{
			output = event->_contentEventType;
		}

		return  rv;
	}

	bool ContentEventTimer::popAndProcessEvent_(void) noexcept
	{
		ContentEventObject* event = _contentEventQueue.top();
		
		ContentEventOverlappedStruct* overlappedStruct = new ContentEventOverlappedStruct;

		overlappedStruct->_contentEventObject = event;
		overlappedStruct->_workType = ThreadWorkType::ContentEvent;

		PostQueuedCompletionStatus(_iocp->_handle, 1, event->_myActorKey.getKeyValue(), &overlappedStruct->_wsaOverlapped);

		_contentEventQueue.pop();
		return true;
	}
}

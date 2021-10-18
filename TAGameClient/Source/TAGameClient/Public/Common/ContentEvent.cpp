#include "Common/ContentEvent.h"



namespace ta
{
	ContentEventObject::ContentEventObject(void) noexcept
	{
		clear();
	}

	ContentEventObject::~ContentEventObject(void) noexcept
	{
	}

	void ContentEventObject::clear(void) noexcept
	{
		_wakeupTime = {};
		_contentEventType = ContentEventType::Count;
		_deltaTime = 0.0f;

		clearDetail();
	}

	void ContentEventObject::clearDetail(void) noexcept
	{
	}

	constexpr bool ContentEventObject::operator<(const ContentEventObject& _left) const noexcept
	{
		return (_wakeupTime > _left._wakeupTime);
	}

	void ContentEventObject::setWakeupTime(const long long delayMilliSec) noexcept
	{
		_wakeupTime = std::chrono::high_resolution_clock::now();
		std::chrono::milliseconds delayTime(delayMilliSec);
		_wakeupTime += delayTime;
		_deltaTime = ((float)delayMilliSec / 1000.0f);
	}
}

namespace ta
{
	ContentEventAiObject::ContentEventAiObject(void) noexcept
	{
	}

	ContentEventAiObject::~ContentEventAiObject(void) noexcept
	{

	}

	void ContentEventAiObject::clearDetail(void) noexcept
	{
		_aiCommandType = AiCommandType::Count;
	}
}


namespace ta
{
	ContentEventBuffObject::ContentEventBuffObject(void) noexcept
	{
	}

	ContentEventBuffObject::~ContentEventBuffObject(void) noexcept
	{
	}

	void ContentEventBuffObject::clearDetail(void) noexcept
	{
		_buffGameDataKey.clear();
	}
}


namespace ta
{
	ContentEventSectorObject::ContentEventSectorObject(void) noexcept
	{
	}

	ContentEventSectorObject::~ContentEventSectorObject(void) noexcept
	{
	}

	void ContentEventSectorObject::clearDetail(void) noexcept
	{
		_sectorEventIndex = 0;
		_isBasicSectorEvent = true;
	}
}


namespace ta
{
	ContentEventOverlappedStruct::ContentEventOverlappedStruct(void) noexcept
	{
		clear();
	}

	ContentEventOverlappedStruct::~ContentEventOverlappedStruct(void) noexcept
	{
		clear();
	}

	void ContentEventOverlappedStruct::clear(void) noexcept
	{
		OverlappedStructBase::clearBase();
		if (nullptr != _contentEventObject)
		{
			delete _contentEventObject;
		}

		_contentEventObject = nullptr;
	}
}



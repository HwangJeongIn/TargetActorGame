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
		_aiCommandType = AiCommandType::Count;
		_deltaTime = 0.0f;

		_sectorEventIndex = 0;
		_isBasicSectorEvent = true;
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



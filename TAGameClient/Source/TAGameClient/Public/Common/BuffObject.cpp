#include "Common/BuffObject.h"
#include "Common/BuffGameDataObject.h"
#include "Common/GameData.h"
#include "Common/CommonApp.h"
#include "Common/ScopedLock.h"


namespace ta
{
	BuffObject::BuffObject(void) noexcept
		: _buffGameData(nullptr)
		, _remainingTime(0.0f)
	{
	}

	BuffObject::~BuffObject(void) noexcept
	{
	}

	bool BuffObject::initializeBuffObject(const BuffGameData* buffGameData) noexcept
	{
		if (nullptr != _buffGameData)
		{
			TA_ASSERT_DEV(false, "비정상입니다");
			return false;
		}

		if (nullptr == buffGameData)
		{
			TA_ASSERT_DEV(false, "비정상입니다");
			return false;
		}

		_buffGameData = buffGameData;
		_remainingTime = buffGameData->_buffObject->_duration;

		return true;
	}

	BuffState BuffObject::doBuff(const ContentParameter& parameter) noexcept
	{
		const uint32 interval = _buffGameData->getInterval();

		if (false == _buffGameData->isOneTime())
		{
			// 지속가능 버프지만 더 이상 지속가능한 시간이 남아있지 않다.
			if (_remainingTime < interval)
			{
				return BuffState::Unsustainable;
			}

			{
				ScopedLock lock(this);
				_remainingTime -= interval;
			}
		}

		const BuffState buffState = _buffGameData->checkAndDoBuff(parameter);

		if (buffState == BuffState::Unsustainable)
		{
			return BuffState::Unsustainable;
		}

		return BuffState::Sustainable;
	}

	bool BuffObject::undoBuff(const ContentParameter& parameter) const noexcept
	{
		return _buffGameData->undoBuff(parameter);
	}
	
	uint32 BuffObject::getInterval(void) const noexcept
	{
		return _buffGameData->getInterval();
	}
	
	bool BuffObject::isOneTime(void) const noexcept
	{
		return _buffGameData->isOneTime();
	}
}

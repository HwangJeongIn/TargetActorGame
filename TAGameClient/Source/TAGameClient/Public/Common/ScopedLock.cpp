#include "Common/ScopedLock.h"
#include "Common/Lockable.h"


namespace ta
{
	ScopedLock::ScopedLock(Lockable* lockOwner, const bool isReadOnly) noexcept
		: _isValid(false)
		, _isReadOnly(isReadOnly)
	{
		if (nullptr == lockOwner || false == lockOwner->isValid())
		{
			TA_ASSERT_DEV(false, "락이 제대로 초기화되지 않았습니다.");
			return;
		}

		_isValid = true;
		_ownerLock = (lockOwner->getLockPtr());
		
		if (true == _isReadOnly)
		{
			AcquireSRWLockShared(_ownerLock.get());
		}
		else
		{
			AcquireSRWLockExclusive(_ownerLock.get());
		}
	}

	ScopedLock::~ScopedLock(void) noexcept
	{
		if (true == _isValid)
		{
			if (true == _isReadOnly)
			{
				ReleaseSRWLockShared(_ownerLock.get());
			}
			else
			{
				ReleaseSRWLockExclusive(_ownerLock.get());
			}
		}
	}
}


#include "Common/Lockable.h"

namespace ta
{
	const bool Lockable::isValid(void) const noexcept
	{
		// 확인결과 슬림락 ptr자체가 초기화하면 0값이 된다.
		return ((nullptr != _lock)/* && (nullptr != _lock->Ptr)*/);
	}

	std::shared_ptr<SRWLOCK> Lockable::getLockPtr(void) noexcept
	{
		if (false == isValid())
		{
			TA_ASSERT_DEV(false, "락이 제대로 초기화되지 않았습니다.");
			return nullptr;
		}
		
		return _lock;
	}

	Lockable::Lockable(void) noexcept
		: _lock(new SRWLOCK)
	{
		//auto test = _lock.get();
		InitializeSRWLock(_lock.get());
		//SRWLOCK a;
		//InitializeSRWLock(&a);


		////AcquireSRWLockShared(_ownerLock.get());
		//AcquireSRWLockExclusive(&a);
		////ReleaseSRWLockShared(_ownerLock.get());
		//AcquireSRWLockExclusive(&a);

		//ReleaseSRWLockExclusive(&a);
	
	}

	Lockable::~Lockable(void) noexcept
	{

	}
}


namespace ta
{
	LockableObject::LockableObject(void) noexcept
	{
	}

	LockableObject::~LockableObject(void) noexcept
	{
	}
}


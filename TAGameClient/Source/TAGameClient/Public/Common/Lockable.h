#pragma once
#include "Common/CommonBase.h"

namespace ta
{
	class Lockable
	{
	public:
		const bool isValid(void) const noexcept;
		std::shared_ptr<SRWLOCK> getLockPtr(void) noexcept;

	protected:
		Lockable(void) noexcept;
		virtual ~Lockable(void) noexcept;

	private:
		std::shared_ptr<SRWLOCK> _lock;
	};
}


namespace ta
{
	class LockableObject : public Lockable
	{
	public:
		LockableObject(void) noexcept;
		virtual ~LockableObject(void) noexcept;
	};
}
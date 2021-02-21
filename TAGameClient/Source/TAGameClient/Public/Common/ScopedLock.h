#pragma once

#include "Common/CommonBase.h"

namespace ta
{
	class Lockable;
}


namespace ta
{
	class ScopedLock
	{
	public:
		ScopedLock(Lockable* lockOwner, const bool isReadOnly = false) noexcept;
		virtual ~ScopedLock(void) noexcept;


	private:
		bool _isValid;
		bool _isReadOnly;
		std::shared_ptr<SRWLOCK> _ownerLock;
	};
}

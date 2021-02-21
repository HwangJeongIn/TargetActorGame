#pragma once

#include "Common/CommonBase.h"

namespace ta
{
	class ActorDataPool;
}

namespace ta
{
	class ActorDataPoolValue
	{
	public:
		ActorDataPoolValue(void) noexcept;
		virtual ~ActorDataPoolValue(void) noexcept;

		const bool getInUse_(void) const noexcept;
		const bool isActive_(void) const noexcept;
		const bool checkState_(const PoolValueState poolValueState) const noexcept;
		
		const uint32 getUseCount(void) const noexcept;
		const uint32 getPoolIndex(void) const noexcept;

		void clearPoolValue_(void) noexcept;

		void updateToPool_(void) noexcept;
		void updateFromPool_(const uint32 poolIndex) noexcept;
		void updatePoolValueState_(const PoolValueState poolValueState) noexcept;

	private:
		PoolValueState _poolValueState;
		uint32 _useCount;
		uint32 _poolIndex;
	};
}
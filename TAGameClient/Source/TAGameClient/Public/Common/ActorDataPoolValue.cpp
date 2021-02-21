#include "Common/ActorDataPoolValue.h"


namespace ta
{
	ActorDataPoolValue::ActorDataPoolValue(void) noexcept
	{
		clearPoolValue_();
	}

	ActorDataPoolValue::~ActorDataPoolValue(void) noexcept
	{
	}

	const bool ActorDataPoolValue::getInUse_(void) const noexcept
	{
		return (PoolValueState::Waiting != _poolValueState);
	}
	
	const bool ActorDataPoolValue::isActive_(void) const noexcept
	{
		return (PoolValueState::Using == _poolValueState);
	}

	const bool ActorDataPoolValue::checkState_(const PoolValueState poolValueState) const noexcept
	{
		return (_poolValueState == poolValueState);
	}

	const uint32 ActorDataPoolValue::getUseCount(void) const noexcept
	{
		return _useCount;
	}

	const uint32 ActorDataPoolValue::getPoolIndex(void) const noexcept
	{
		return _poolIndex;
	}

	void ActorDataPoolValue::clearPoolValue_(void) noexcept
	{
		updatePoolValueState_(PoolValueState::Waiting);
		_poolIndex = 0;
		_useCount = 0;
	}

	void ActorDataPoolValue::updateToPool_(void) noexcept
	{
		updatePoolValueState_(PoolValueState::Waiting);
		_poolIndex = 0;
	}

	void ActorDataPoolValue::updateFromPool_(const uint32 poolIndex) noexcept
	{
		updatePoolValueState_(PoolValueState::Using);
		_poolIndex = poolIndex;
		++_useCount;
	}

	void ActorDataPoolValue::updatePoolValueState_(const PoolValueState poolValueState) noexcept
	{
		_poolValueState = poolValueState;
	}
}


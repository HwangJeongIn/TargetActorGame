#include "Common/PathPoint.h"


namespace ta
{
	PathPoint::PathPoint(const Vector& position /*= Vector::Zero*/) noexcept
		: _position(position)
	{
	}

	PathPoint::~PathPoint(void) noexcept
	{
	}

	const Vector& PathPoint::getPosition(void) const noexcept
	{
		return _position;
	}
}


#include "NavMeshPoint.h"


namespace ta
{
	NavMeshPoint::NavMeshPoint(void) noexcept
		: _position(Vector::Zero)
		, _polyRef(INVALID_NAVNODEREF)
	{
	}
	
	NavMeshPoint::NavMeshPoint(const Vector& position, dtPolyRef polyRef) noexcept
		: _position(position), _polyRef(polyRef) 
	{
	}
	
	bool NavMeshPoint::hasPolyRef(void) const noexcept
	{
		return _polyRef != INVALID_NAVNODEREF;
	}
}


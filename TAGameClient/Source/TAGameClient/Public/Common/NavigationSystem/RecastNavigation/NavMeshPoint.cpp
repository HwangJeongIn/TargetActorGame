#include "RecastNavigation/NavMeshPoint.h"


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
	const Vector& NavMeshPoint::getPosition(void) const noexcept
	{
		return _position;
	}

	const dtPolyRef& NavMeshPoint::getPolyRef(void) const noexcept
	{
		return _polyRef;
	}

	void NavMeshPoint::setPosition(const Vector& position) noexcept
	{
		_position = position;
	}

	void NavMeshPoint::setPolyRef(const dtPolyRef& polyRef) noexcept
	{
		_polyRef = polyRef;
	}
}


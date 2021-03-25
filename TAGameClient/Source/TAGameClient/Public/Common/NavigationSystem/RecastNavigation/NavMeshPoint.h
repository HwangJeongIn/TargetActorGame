#pragma once

#include "Common/CommonBase.h"
#include "Common/Vector.h"
#include "RecastNavigation/RecastNavigationSystemInclude.h"
#include "RecastNavigation/Detour/DetourNavMesh.h"


namespace ta
{
	class NavMeshPoint
	{
	public:
		Vector _position;
		dtPolyRef _polyRef;

		NavMeshPoint(void) noexcept;
		explicit NavMeshPoint(const Vector& position, dtPolyRef polyRef = INVALID_NAVNODEREF) noexcept;

		bool hasPolyRef(void) const noexcept;
	};
}

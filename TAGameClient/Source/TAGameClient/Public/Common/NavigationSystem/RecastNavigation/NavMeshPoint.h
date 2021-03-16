#pragma once

#include "Common/CommonBase.h"
#include "Common/Vector.h"
#include "Detour/DetourNavMesh.h"
#include "RecastNavigationSystemInclude.h"


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

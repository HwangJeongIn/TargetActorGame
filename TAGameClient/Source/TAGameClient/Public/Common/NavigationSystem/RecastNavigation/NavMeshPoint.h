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
		NavMeshPoint(void) noexcept;
		explicit NavMeshPoint(const Vector& position, dtPolyRef polyRef = INVALID_NAVNODEREF) noexcept;

		bool hasPolyRef(void) const noexcept;

		const Vector& getPosition(void) const noexcept;
		const dtPolyRef& getPolyRef(void) const noexcept;

		void setPosition(const Vector& position) noexcept;
		void setPolyRef(const dtPolyRef& polyRef) noexcept;

	private:
		Vector _position;
		dtPolyRef _polyRef;

	};
}

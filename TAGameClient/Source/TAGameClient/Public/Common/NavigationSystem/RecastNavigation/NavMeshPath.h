#pragma once

#include "Common/CommonBase.h"
#include "NavMeshPoint.h"
#include <vector>


namespace ta
{
	class NavMeshPath
	{
	public:
		NavMeshPath(void) noexcept;
		virtual ~NavMeshPath(void) noexcept;

		bool hasPathPoint(void) noexcept;

		std::vector<NavMeshPoint*>& getPath(void) noexcept;
		std::vector<dtPolyRef>& getPathCorridor(void) noexcept;
		std::vector<float>& getPathCorridorCost(void) noexcept;
		std::vector<uint32>& getCustomLinkIds(void) noexcept;


		void clearPath(void) noexcept;

		void addPathPoint(NavMeshPoint* point) noexcept;
		void addPathPoint(const Vector& position, const dtPolyRef& polyRef = INVALID_NAVNODEREF) noexcept;

		NavMeshPoint* popLastPathPoint(void) noexcept;
		NavMeshPoint* popFristPathPoint(void) noexcept;


		void addPathCorridor(const dtPolyRef& corridor) noexcept;
		dtPolyRef getPathCorridor(const uint32 index) const noexcept;

		void addPathCorridorCost(float corridorCost) noexcept;
		float getPathCorridorCost(const uint32 index) const noexcept;


	private:
		std::vector<NavMeshPoint*> _path;

		/** sequence of navigation mesh poly ids representing an obstacle-free navigation corridor */
		std::vector<dtPolyRef> _pathCorridor;

		/** for every poly in PathCorridor stores traversal cost from previous navpoly */
		std::vector<float> _pathCorridorCost;

		/** set of unique link Ids */
		std::vector<uint32> _customLinkIds;
	};
}

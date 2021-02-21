#pragma once

#include "Common/CommonBase.h"
#include "Common/Vector.h"
#include <vector>


namespace ta
{
	class NavigationNodeData
	{
	public:
		NavigationNodeData(void) noexcept;
		virtual ~NavigationNodeData(void) noexcept;

		const std::vector<NavigationNodeData*>& getAdjacentNodeData(void) noexcept;
		const Vector& getPosition(void) noexcept;

	private:
		Vector _position;
		std::vector<NavigationNodeData*> _adjacentNodeData;
	};
}


namespace ta
{
	class NavigationSystem
	{
	public:
		NavigationSystem(void) noexcept;
		virtual ~NavigationSystem(void) noexcept;

		bool findPath(const Vector& startPoint, const Vector& endPoint, std::vector<NavigationNodeData*>& output) noexcept;
		NavigationNodeData* findNearestNode(const Vector& point) noexcept;


	private:
		std::vector<NavigationNodeData*> _allNavigationNodeData;
	};
}
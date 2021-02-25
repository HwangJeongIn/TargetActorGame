#include "Server/Pathfinding.h"
#include <unordered_set>


namespace ta
{
	NavigationNodeData::NavigationNodeData(void) noexcept
	{
	}

	NavigationNodeData::~NavigationNodeData(void) noexcept
	{
	}

	const std::vector<NavigationNodeData*>& NavigationNodeData::getAdjacentNodeData(void) noexcept
	{
		return _adjacentNodeData;
	}

	const Vector& NavigationNodeData::getPosition(void) noexcept
	{
		return _position;
	}

}


namespace ta
{
	NavigationSystem::NavigationSystem(void) noexcept
	{
	}

	NavigationSystem::~NavigationSystem(void) noexcept
	{
	}

	bool NavigationSystem::findPath(const Vector& startPoint, const Vector& endPoint, std::vector<NavigationNodeData*>& output) noexcept
	{
		NavigationNodeData* startNode = findNearestNode(startPoint);
		NavigationNodeData* endNode = findNearestNode(endPoint);

		if (nullptr == startNode
			|| nullptr == endNode)
		{
			TA_ASSERT_DEV(false, "비정상입니다");
			return false;
		}

		std::unordered_set<NavigationNodeData*> openSet;
		std::unordered_set<NavigationNodeData*> closeSet;

		//closeSet.insert
		//
		//
		//const std::vector<NavigationNodeData*>& startAdjacentNodeData = startNode->getAdjacentNodeData();


		return false;
	}

	NavigationNodeData* NavigationSystem::findNearestNode(const Vector& point) noexcept
	{
		// 비정상이면 nullptr
		return nullptr;
	}


}

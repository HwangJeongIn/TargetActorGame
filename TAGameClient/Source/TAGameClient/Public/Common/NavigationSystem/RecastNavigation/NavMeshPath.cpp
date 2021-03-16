#include "NavMeshPath.h"

namespace ta
{
	NavMeshPath::NavMeshPath(void) noexcept
	{
	}

	NavMeshPath::~NavMeshPath(void) noexcept
	{
		clearPath();
	}

	bool NavMeshPath::hasPathPoint(void) noexcept
	{
		return (0 != _path.size());
	}

	std::vector<NavMeshPoint*>& NavMeshPath::getPath(void) noexcept
	{
		return _path;
	}

	std::vector<dtPolyRef>& NavMeshPath::getPathCorridor(void) noexcept
	{
		return _pathCorridor;
	}

	std::vector<float>& NavMeshPath::getPathCorridorCost(void) noexcept
	{
		return _pathCorridorCost;
	}

	std::vector<uint32>& NavMeshPath::getCustomLinkIds(void) noexcept
	{
		return _customLinkIds;
	}

	void NavMeshPath::clearPath(void) noexcept
	{
		const uint32 count = _path.size();
		for (uint32 index = 0; index < count; ++index)
		{
			if (nullptr != _path[index])
			{
				delete _path[index];
			}
		}

		_path.clear();
		_pathCorridor.clear();
		_pathCorridorCost.clear();
		_customLinkIds.clear();
	}

	void NavMeshPath::addPathPoint(NavMeshPoint* point) noexcept
	{
		if (nullptr == point)
		{
			return;
		}

		_path.push_back(point);
	}

	void NavMeshPath::addPathPoint(const Vector& position, const dtPolyRef& polyRef) noexcept
	{
		_path.push_back(new NavMeshPoint(position, polyRef));
	}

	NavMeshPoint* NavMeshPath::popLastPathPoint(void) noexcept
	{
		if (false == hasPathPoint())
		{
			return nullptr;
		}

		NavMeshPoint * last = _path.back();
		_path.pop_back();
		return last;
	}

	NavMeshPoint* NavMeshPath::popFristPathPoint(void) noexcept
	{
		if (false == hasPathPoint())
		{
			return nullptr;
		}

		NavMeshPoint* frist = _path.front();
		_path.erase(_path.begin());
		return frist;
	}

	void NavMeshPath::addPathCorridor(const dtPolyRef& corridor) noexcept
	{
		_pathCorridor.push_back(corridor);
	}

	dtPolyRef NavMeshPath::getPathCorridor(const uint32 index) const noexcept
	{
		TA_ASSERT_DEV(index < _pathCorridor.size(), "비정상");
		return _pathCorridor[index];
	}

	void NavMeshPath::addPathCorridorCost(float corridorCost) noexcept
	{
		_pathCorridorCost.push_back(corridorCost);
	}

	float NavMeshPath::getPathCorridorCost(const uint32 index) const noexcept
	{
		TA_ASSERT_DEV(index < _pathCorridorCost.size(), "비정상");
		return _pathCorridorCost[index];
	}

}

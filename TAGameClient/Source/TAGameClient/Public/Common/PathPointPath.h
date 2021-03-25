#pragma once

#include "Common/CommonBase.h"
#include "RecastNavigation/NavMeshPoint.h"
#include <vector>


namespace ta
{
	class ActorKey;
	class PathPointIndex;
	class PathPoint;
}


namespace ta
{
	class PathPointPath
	{
	public:
		explicit PathPointPath(const bool isCirculationPath = false) noexcept;
		virtual ~PathPointPath(void) noexcept;

		bool hasPathPoint(void) noexcept;

		const std::vector<PathPoint*>& getPath(void) const noexcept;
		void clearPath(void) noexcept;
		
		void addPathPoint(const Vector& position) noexcept;
		
		const PathPoint* getNearestPathPoint(const Vector& currentPosition, PathPointIndex& pathPointIndex) const noexcept;
		const PathPoint* getNextPathPoint(PathPointIndex& currentPathPointIndex) noexcept;

	private:
		const uint32 getPrevIndex(const uint32& index) const noexcept;
		const uint32 getNextIndex(const uint32& index) const noexcept;

	private:
		std::vector<PathPoint*> _path;
		bool _isCirculationPath;	// 아직 미구현	// 순환(true) : 1 > 2 > 3 > 1 > 2... 반복
									//				// 기본동작(false) : 1 > 2 > 3 > 2 > 1... 반복
	};
}


namespace ta
{
	class PathPointIndex
	{
	public:
		friend PathPointPath;

		PathPointIndex(void) noexcept;
		~PathPointIndex(void) noexcept;

		const bool isValid(void) const noexcept;

	private:
		void clear(void) noexcept;

	private:
		int32 _currentIndex;
		int32 _prevIndex;
	};
}

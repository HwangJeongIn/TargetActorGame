#pragma once

#include "Common/CommonBase.h"
#include "Common/KeyDefinition.h"
#include "Common/PathPointPath.h"
#include "RecastNavigation/NavMeshPath.h"


namespace ta
{
	class PathPointPath;
	class PathPoint;
}

namespace ta
{
	class AiPathPointPath
	{
	public:
		explicit AiPathPointPath(const PathPointPath& pathPointPath, const ActorKey& owner) noexcept;
		virtual ~AiPathPointPath(void) noexcept;

		// output으로 크기와 방향을 가진 벡터가 나온다. // speed = cm/s이다 // ue4에서 Unreal Units/second
		bool findNextPoint(const Vector& currentPos, const float speed, const float sec, Vector& output) noexcept;
		bool hasPath(void) noexcept;

	private:
		bool processNextPathPoint(const Vector& currentPos, const float speed, const float sec, Vector& output) noexcept;

		void clear(void) noexcept;
		void clearNavMeshPath(void) noexcept;

	private:
		ActorKey _owner;
		const PathPointPath* _pathPointPath;
		PathPointIndex _currentPathPointPathIndex;
		const PathPoint* _currentDestination;


		NavMeshPath _navMeshPath; // 다음 포인트로 PathPoint로 가기 위한 Path
		uint32 _currentNavMeshPathIndex;
		const NavMeshPoint* _currentNearDestination;
	};
}
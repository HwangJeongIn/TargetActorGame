#include "Common/AiPathPointPath.h"
#include "Common/CommonMoveActorSystem.h"
#include "Common/GetComponentAndSystem.h"
#include "Common/PathPoint.h"


namespace ta
{
	AiPathPointPath::AiPathPointPath(const PathPointPath& pathPointPath, const ActorKey& owner) noexcept
	{
		clear();

		// 무조건 받았으니 널체크는 안한다.
		_pathPointPath = &pathPointPath;
		_owner = owner;
	}

	AiPathPointPath::~AiPathPointPath(void) noexcept
	{
		clear();
	}
	
	bool AiPathPointPath::findNextPoint(const Vector& currentPos, const float speed, const float sec, Vector& output) noexcept
	{
		if (false == _currentPathPointPathIndex.isValid()) // 맨처음 들어 왔을 때 
		{
			_currentDestination = _pathPointPath->getNearestPathPoint(currentPos, _currentPathPointPathIndex);
			if (nullptr == _currentDestination)
			{
				TA_LOG_DEV("가까운 PathPoint를 구하지 못했습니다.");
				return false;
			}

			CommonMoveActorSystem* actorMoveSystem = GetActorSystem<CommonMoveActorSystem>();
			if (false == actorMoveSystem->findPath(_owner, currentPos, _currentDestination->getPosition(), _navMeshPath))
			{
				TA_ASSERT_DEV(false, "경로를 찾지 못했습니다.");
				return false;
			}

			if (false == _navMeshPath.hasPathPoint())
			{
				TA_ASSERT_DEV(false, "경로를 찾지 못했습니다.");
				return false;
			}

			// 새로 찾았으니까 0부터시작
			_currentNavMeshPathIndex = 0;
			_currentNearDestination = _navMeshPath.getPathPoint(_currentNavMeshPathIndex);
		}

		// 재귀적 처리
		if (false == processNextPathPoint(currentPos, speed, sec, output))
		{
			TA_ASSERT_DEV(false, "비정상");
			return false;
		}

		return true;
	}

	bool AiPathPointPath::processNextPathPoint(const Vector& currentPos, const float speed, const float sec, Vector& output) noexcept
	{
		const Vector& nearDestination = _currentNearDestination->getPosition();
		Vector direction = nearDestination - currentPos;
		float distanceToNearDestinationSquared = direction.sizeSquared();
		float distacneToMove = speed * sec;

		// 다음 NavPathPoint으로 넘어가야할 경우
		if ((AllowedRangeToDestinationSquared >= distanceToNearDestinationSquared) // 도착 허용범위내로 들어오거나
			|| ((distacneToMove * distacneToMove) > distanceToNearDestinationSquared)) // 이번에 갈거리보다 가까울 때 다음 NavPathPoint를 찾는다.
		{
			if ((_navMeshPath.getPathPointCount() - 1) != _currentNavMeshPathIndex) // 마지막이 아닐때
			{
				++_currentNavMeshPathIndex;
				_currentNearDestination = _navMeshPath.getPathPoint(_currentNavMeshPathIndex);
				return processNextPathPoint(currentPos, speed, sec, output);
			}
			
			// 마지막 NavPathPoint다. 새로 다음 PathPoint까지 경로를 찾자
			clearNavMeshPath();

			// 새로운 지점 찍어줘야한다.
			_currentDestination = _pathPointPath->getNextPathPoint(_currentPathPointPathIndex);
			if (nullptr == _currentDestination)
			{
				TA_ASSERT_DEV(false, "코드버그");
				return false;
			}

			CommonMoveActorSystem* actorMoveSystem = GetActorSystem<CommonMoveActorSystem>();
			if (false == actorMoveSystem->findPath(_owner, currentPos, _currentDestination->getPosition(), _navMeshPath))
			{
				TA_ASSERT_DEV(false, "경로를 찾지 못했습니다.");
				return false;
			}

			if (false == _navMeshPath.hasPathPoint())
			{
				TA_ASSERT_DEV(false, "경로를 찾지 못했습니다.");
				return false;
			}

			// 새로 찾았으니까 0부터시작
			_currentNavMeshPathIndex = 0;
			_currentNearDestination = _navMeshPath.getPathPoint(_currentNavMeshPathIndex);

			return processNextPathPoint(currentPos, speed, sec, output);
		}
		else // 그냥 이동하면 되는 경우
		{
			direction.normalize();
			output = direction * distacneToMove;
		}

		return true;
	}
	
	void AiPathPointPath::clear(void) noexcept
	{
		_owner.clear();
		_pathPointPath = nullptr;
		_currentPathPointPathIndex.clear();
		_currentDestination = nullptr;

		clearNavMeshPath();
	}

	void AiPathPointPath::clearNavMeshPath(void) noexcept
	{
		_navMeshPath.clearPath();
		_currentNavMeshPathIndex = 0;
		_currentNearDestination = nullptr;
	}
}


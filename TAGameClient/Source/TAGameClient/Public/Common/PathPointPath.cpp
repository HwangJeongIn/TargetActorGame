#include "Common/PathPointPath.h"
#include "Common/PathPoint.h"
#include "Common/KeyDefinition.h"
#include "Common/GetComponentAndSystem.h"
#include "Common/CommonMoveActorComponent.h"
#include "Common/CommonApp.h"
#include "Common/ScopedLock.h"


namespace ta
{
	PathPointIndex::PathPointIndex(void) noexcept
	{
		clear();
	}

	PathPointIndex::~PathPointIndex(void) noexcept
	{
	}

	const bool PathPointIndex::isValid(void) const noexcept
	{
		return (-1 != _currentIndex);
	}
	
	void PathPointIndex::clear(void) noexcept
	{
		_currentIndex = -1;
		_prevIndex = -1;
	}
}


namespace ta
{
	PathPointPath::PathPointPath(const bool isCirculationPath /*= false*/) noexcept
		: _isCirculationPath(isCirculationPath)
	{
	}

	PathPointPath::~PathPointPath(void) noexcept
	{
	}

	bool PathPointPath::hasPathPoint(void) noexcept
	{
		return (0 != _path.size());
	}

	const std::vector<PathPoint*>& PathPointPath::getPath(void) const noexcept
	{
		return _path;
	}

	void PathPointPath::clearPath(void) noexcept
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
	}

	void PathPointPath::addPathPoint(const Vector& position) noexcept
	{
		_path.push_back(new PathPoint(position));
	}

	const PathPoint* PathPointPath::getNearestPathPoint(const Vector& currentPosition, PathPointIndex& pathPointIndex) const noexcept
	{
		// const ActorKey& actorKey 원래 파라미터로 받아서 해당 액터 위치 받아서 처리하려 했으나 최대한 연관없이 사용하기 위해서 Vector로 변경함
		//if (false == actorKey.isValid())
		//{
		//	TA_ASSERT_DEV(false, "비정상입니다.");
		//	return nullptr;
		//}

		float currentDistanceSq = (MaxDistanceFromActorToPathPoint * MaxDistanceFromActorToPathPoint);
		const uint32 count = _path.size();
		int32 resultPathPointIndex = -1;
		for (uint32 index = 0; index < count; ++index)
		{
			const float currentIndexPathPointDistanceSq = (currentPosition - _path[index]->getPosition()).sizeSquared();
			if (currentIndexPathPointDistanceSq <= currentDistanceSq)
			{
				currentDistanceSq = currentIndexPathPointDistanceSq;
				resultPathPointIndex = index;
			}
		}

		if (-1 == resultPathPointIndex)
		{
			TA_ASSERT_DEV(false, "MaxDistanceFromActorToPathPoint거리보다 멀어서 가까운 PathPoint를 구하지 못했습니다.");
			pathPointIndex.clear();
			
			return nullptr;
		}

		pathPointIndex._currentIndex = resultPathPointIndex;
		pathPointIndex._prevIndex = getPrevIndex(resultPathPointIndex);

		return _path[resultPathPointIndex];
	}

	const PathPoint* PathPointPath::getNextPathPoint(PathPointIndex& currentPathPointIndex) noexcept
	{
		if (false == currentPathPointIndex.isValid())
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return nullptr;
		}

		bool isForwardDirection = false;

		if (true == _isCirculationPath)
		{
			if (currentPathPointIndex._prevIndex != getPrevIndex(currentPathPointIndex._currentIndex))
			{
				// 역방향이거나 코드버그거나 // 순환경로에서는 역방향 개념이없다
				TA_ASSERT_DEV(false, "비정상입니다.");
				return nullptr;
			}

			isForwardDirection = true;
		}
		else
		{
			if (currentPathPointIndex._prevIndex == getPrevIndex(currentPathPointIndex._currentIndex))
			{
				// 정방향
				isForwardDirection = true;
			}
			else if (currentPathPointIndex._prevIndex == getNextIndex(currentPathPointIndex._currentIndex))
			{
				// 역방향
				isForwardDirection = false;
			}
			else
			{
				// 코드버그
				TA_ASSERT_DEV(false, "비정상입니다.");
				return nullptr;
			}
		}

		currentPathPointIndex._prevIndex = currentPathPointIndex._currentIndex;

		if (true == isForwardDirection) // 정방향
		{
			currentPathPointIndex._currentIndex = getNextIndex(currentPathPointIndex._currentIndex);
		}
		else // 역방향
		{
			currentPathPointIndex._currentIndex = getPrevIndex(currentPathPointIndex._currentIndex);
		}

		return _path[currentPathPointIndex._currentIndex];
	}

	const uint32 PathPointPath::getPrevIndex(const uint32& index) const noexcept
	{
		return ((0 == index) ? (_path.size() - 1) : (index - 1));
	}

	const uint32 PathPointPath::getNextIndex(const uint32& index) const noexcept
	{
		return ((index + 1) % _path.size());
	}
}


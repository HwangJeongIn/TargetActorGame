#include "Common/Sector.h"
#include <cmath>
#include "Common/Vector.h"
#include "Common/CommonActor.h"
#include "Common/ScopedLock.h"
#include "Common/CommonApp.h"
#include "Common/CommonActorManager.h"
#include "Common/SectorProcessor.h"

#include <cstdlib>


namespace ta
{
	Sector* GetSector(const SectorKey& sectorKey) noexcept
	{
		return g_app->getActorManager()->getSector(sectorKey);
	}

	bool CheckIsInSector(const Vector& position) noexcept
	{
#ifdef GetNearSector_v1
		//return (abs(position._x) < MaxPositionValueExcludingSideSector) && (abs(position._y) < MaxPositionValueExcludingSideSector);
		return (abs(position._x) < MaxPositionValue) && (abs(position._y) < MaxPositionValue);
#endif

#ifdef GetNearSector_v2
		return (abs(position._x) < MaxPositionValue) && (abs(position._y) < MaxPositionValue);
#endif
	}

	SectorKey GetSectorIndex(const Vector& position) noexcept
	{
		if (false == CheckIsInSector(position))
		{
			TA_ASSERT_DEV(false, "비정상입니다.")
			return SectorKey();
		}

		/*
				+x

			-y	 O	+y

				-x
		*/

		// 인덱스는 섹터 사격형의 왼쪽아래 기준이다.

		uint32 indexX = 0;
		uint32 indexY = 0;
		// 인덱스는 무조건 올려야한다. ex) SectorSize = 300일때, // 좀 더 계산이 들어가지만 기본적으로 다음과 같다.
		//								 Position(-200, 100) => SectorIndex(0,1)
		//								 Position(-301, 100) => SectorIndex(-1,1)
		//								 Position(400, 500) => SectorIndex(2,2)

		if (0.0f < position._x)
		{
			// x값이 클수록 높아지기 때문에 x인덱스는 작아진다. // 남으면 올림해야한다.
			indexX = (HalfCountOfOneSideSectors)+(uint32)floor(position._x / SectorSize);
		}
		else
		{
			indexX = (HalfCountOfOneSideSectors)-(uint32)abs(floor(position._x / SectorSize));
		}

		if (0.0f < position._y)
		{
			indexY = (HalfCountOfOneSideSectors)+(uint32)floor(position._y / SectorSize);
		}
		else
		{
			indexY = (HalfCountOfOneSideSectors)-(uint32)abs(floor(position._y / SectorSize));
		}

		return SectorKey(indexX * CountOfOneSideSectors + indexY);
	}

	bool GetSectorIndexXY(const SectorKey& sectorKey, int32& indexX, int32& indexY) noexcept
	{
		if (false == sectorKey.isValid())
		{
			TA_ASSERT_DEV(false, "키값이 유효하지 않습니다.")
			return false;
		}

		const int32 index = sectorKey.getKeyValue();
		if (CountOfSectors <= index)
		{
			TA_ASSERT_DEV(false, "범위를 벗어나는 키값입니다.")
			return false;
		}

		indexX = index / CountOfOneSideSectors;
		indexY = index % CountOfOneSideSectors;

		return true;
	}

	SectorKey GetSectorKey(const int32& indexX, const int32& indexY) noexcept
	{
		if (((int32)HalfCountOfOneSideSectors <= indexX)
			|| ((int32)HalfCountOfOneSideSectors <= indexY)
			|| (-(int32)HalfCountOfOneSideSectors > indexX)
			|| (-(int32)HalfCountOfOneSideSectors > indexY))
		{
			return SectorKey();
		}

		return SectorKey(indexX * CountOfOneSideSectors + indexY);
	}

	bool GetSectorCenterPosition(const SectorKey& sectorKey, Vector& output) noexcept
	{
		int32 indexX = -1;
		int32 indexY = -1;

		if (false == GetSectorIndexXY(sectorKey, indexX, indexY))
		{
			TA_ASSERT_DEV(false, "비정상입니다.")
			return false;
		}

		// 0번인덱스를 0,0로 보고 해당 인덱스 오른쪽 아래 좌표 구하고 섹터 중앙 좌표 구하고 // x는 음수값 y는 양수값이 나온다.
		// 좌표평면으로 이동시키자  최종 좌표 = (finalX + SectorSize * HalfCountOfOneSideSectors, finalY - SectorSize * HalfCountOfOneSideSectors)

		////float finalX = (-indexX * SectorSize) + (HalfCountOfOneSideSectors * SectorSize) + (0.5f * SectorSize);
		//float finalX = (-indexX + 0.5f + HalfCountOfOneSideSectors ) * SectorSize;
		////float finalY = (indexY * SectorSize) - (HalfCountOfOneSideSectors * SectorSize) - (0.5f * SectorSize);
		//float finalY = (indexY - 0.5f - HalfCountOfOneSideSectors) * SectorSize;


		//float finalX = (-indexX * SectorSize) + (HalfCountOfOneSideSectors * SectorSize) + (0.5f * SectorSize);
		float finalX = (indexX + 0.5f - HalfCountOfOneSideSectors) * SectorSize;
		//float finalY = (indexY * SectorSize) - (HalfCountOfOneSideSectors * SectorSize) - (0.5f * SectorSize);
		float finalY = (indexY + 0.5f - HalfCountOfOneSideSectors) * SectorSize;

		output._x = finalX;
		output._y = finalY;

		return true;
	}

	bool GetSectorRandomPosition(const SectorKey& sectorKey, Vector& output) noexcept
	{
		Vector temp;
		if (false == GetSectorCenterPosition(sectorKey, temp))
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		int32 offsetX = rand() % (int)(SectorHalfSize);
		int32 offsetY = rand() % (int)(SectorHalfSize);

		output._x = temp._x + (float)(offsetX);
		output._y = temp._y + (float)(offsetY);

		return true;
	}

	bool GetNearSectors(const SectorKey& sectorKey, std::unordered_set<SectorKey>& output) noexcept
	{
		int32 indexX = -1;
		int32 indexY = -1;

		if (false == GetSectorIndexXY(sectorKey, indexX, indexY))
		{
			TA_ASSERT_DEV(false, "비정상입니다.")
			return false;
		}

		output.clear();

		/*
		   0 1 2
		   3 4 5
		   6 7 8
		*/

		// 액터는 경계에 못들어간다
#ifdef GetNearSector_v1

		//const int32 indexX_p1 = indexX + 1;
		//const int32 indexX_m1 = indexX - 1;

		//const int32 indexY_p1 = indexY + 1;
		//const int32 indexY_m1 = indexY - 1;


		const bool isOnLowerBoundX = indexX <= 0;
		const bool isOnUpperBoundX = indexX >= CountOfOneSideSectors;

		const bool isOnLowerBoundY = indexY <= 0;
		const bool isOnUpperBoundY = indexY >= CountOfOneSideSectors; 

		
		if (false == isOnLowerBoundX)
		{
			output.insert((indexX - 1) * CountOfOneSideSectors + (indexY));

			if (false == isOnLowerBoundY)
			{
				output.insert((indexX - 1) * CountOfOneSideSectors + (indexY - 1));
			}
			
			if (false == isOnUpperBoundY)
			{
				output.insert((indexX - 1) * CountOfOneSideSectors + (indexY + 1));
			}
		}
		
		if (false == isOnUpperBoundX)
		{
			output.insert((indexX + 1) * CountOfOneSideSectors + (indexY));

			if (false == isOnLowerBoundY)
			{
				output.insert((indexX + 1) * CountOfOneSideSectors + (indexY - 1));
			}

			if (false == isOnUpperBoundY)
			{
				output.insert((indexX + 1) * CountOfOneSideSectors + (indexY + 1));
			}
		}


		output.insert((indexX)*CountOfOneSideSectors + (indexY));
		if (false == isOnLowerBoundY)
		{
			output.insert((indexX) * CountOfOneSideSectors + (indexY - 1));
		}

		if (false == isOnUpperBoundY)
		{
			output.insert((indexX) * CountOfOneSideSectors + (indexY + 1));
		}

		return true;
#endif

		// 액터가 경계 섹터에 들어갈 수 있을 때
#ifdef GetNearSector_v2

		// index값이 (CountOfOneSideSectors - 1) or 0일때 각 경계에 위치
		// vector에 어떤걸 넣어야하는지 플래그로 표시하자

		const uint32 nearSectorCount = 9;

		std::vector<bool> insertFlags(nearSectorCount, true);
		std::vector<SectorKey> insertValues;
		insertValues.reserve(nearSectorCount);
		{
			insertValues.emplace_back((indexX - 1) * CountOfOneSideSectors + (indexY - 1));		// 0
			insertValues.emplace_back((indexX - 1) * CountOfOneSideSectors + (indexY));			// 1
			insertValues.emplace_back((indexX - 1) * CountOfOneSideSectors + (indexY + 1));		// 2
			insertValues.emplace_back((indexX)*CountOfOneSideSectors + (indexY - 1));			// 3
			insertValues.emplace_back((indexX)*CountOfOneSideSectors + (indexY));				// 4
			insertValues.emplace_back((indexX)*CountOfOneSideSectors + (indexY + 1));			// 5
			insertValues.emplace_back((indexX + 1) * CountOfOneSideSectors + (indexY - 1));		// 6
			insertValues.emplace_back((indexX + 1) * CountOfOneSideSectors + (indexY));			// 7
			insertValues.emplace_back((indexX + 1) * CountOfOneSideSectors + (indexY + 1));		// 8
		}


		if (0 == indexX)
		{
			// 0 1 2번 빼야한다.
			insertFlags[0] = false;
			insertFlags[1] = false;
			insertFlags[2] = false;
		}
		else if ((CountOfOneSideSectors - 1) == indexX)
		{
			// 5 6 7번 빼야한다.
			insertFlags[5] = false;
			insertFlags[6] = false;
			insertFlags[7] = false;
		}

		if (0 == indexY)
		{
			// 0 3 5번 빼야한다.
			insertFlags[0] = false;
			insertFlags[3] = false;
			insertFlags[5] = false;
		}
		else if ((CountOfOneSideSectors - 1) == indexY)
		{
			// 2 4 7번 빼야한다.
			insertFlags[2] = false;
			insertFlags[4] = false;
			insertFlags[7] = false;
		}

		for (uint32 index = 0; index < nearSectorCount; ++index)
		{
			if (true == insertFlags[index])
			{
				output.insert(insertValues[index]);
			}
		}

		return true;
#endif

	}

	void CompareAndGetDifferentSectors(const std::unordered_set<SectorKey>& fromSectors
									   , const std::unordered_set<SectorKey>& toSectors
									   , std::vector<SectorKey>& addedSectors
									   , std::vector<SectorKey>& removedSectors) noexcept
	{
		addedSectors.clear();
		removedSectors.clear();


		const std::unordered_set<SectorKey>::iterator fromSectorsEnd = fromSectors.end();
		const std::unordered_set<SectorKey>::iterator toSectorsEnd = toSectors.end();

		// added sector list
		{
			std::unordered_set<SectorKey>::iterator it = toSectors.begin();
			for (; it != toSectorsEnd; ++it)
			{
				if (fromSectorsEnd == toSectors.find(*it))
				{
					// 새로운 섹터에는 있지만 예전 섹터에는 없는 섹터 => added sector
					addedSectors.push_back(*it);
				}
			}
		}

		// removed sector list
		{
			std::unordered_set<SectorKey>::iterator it = fromSectors.begin();
			for (; it != fromSectorsEnd; ++it)
			{
				if (toSectorsEnd == toSectors.find(*it))
				{
					// 이전에는 있었는데 지금은 없는 섹터 => removed sector
					removedSectors.push_back(*it);
				}
			}
		}
	}
}


namespace ta
{
	Sector::Sector(void) noexcept
	{
	}

	Sector::~Sector(void) noexcept
	{

	}

	bool Sector::initialize(void) noexcept
	{
		return true;
	}

	bool Sector::open(void) noexcept
	{
		return true;
	}

	void Sector::close(void) noexcept
	{
	}

	void Sector::setSectorKey(const SectorKey& sectorKey) noexcept
	{
		_sectorKey = sectorKey;
	}

	bool Sector::enterSector(CommonActor* actor) noexcept
    {
	
		{
			ScopedLock sectorLock(this);

			if (false == addActor_(actor->getActorKey()))
			{
				TA_ASSERT_DEV(false, "비정상입니다.")
				return false;
			}
		}

        return true;
    }

	bool Sector::exitSector(CommonActor* actor) noexcept
	{
		{
			ScopedLock sectorLock(this);

			if (false == removeActor_(actor->getActorKey()))
			{
				TA_ASSERT_DEV(false, "비정상입니다.")
					return false;
			}
		}

		return true;
	}

	const std::unordered_set<ActorKey>& Sector::getActors_(void) const noexcept
	{
		return _actors;
	}

	ActorKey Sector::getTargetActorFromSector(const std::vector<ActorType>& actorTypes) noexcept
	{
		ScopedLock lock(this, true);
		std::unordered_set<ActorKey>::iterator it = _actors.begin();
		const std::unordered_set<ActorKey>::iterator end = _actors.end();
		ActorType actorType = ActorType::Count;
		const uint32 count = actorTypes.size();

		for (; it != end; ++it)
		{
			CommonActor* targetActor = g_app->getActorManager()->getActor(*it, true);
			if (nullptr == targetActor)
			{
				TA_ASSERT_DEV(false, "비정상입니다.");
				continue;
			}

			actorType = targetActor->getActorType();
			for (uint32 index = 0; index < count; ++index)
			{
				if (actorType == actorTypes[index])
				{
					return *it;
				}
			}
		}

		return ActorKey();
	}

	void Sector::getTargetActorsFromSector(const ActorType& actorType, std::unordered_set<ActorKey>& output) noexcept
	{
		std::vector<ActorType> actorTypes;
		actorTypes.reserve(1);
		actorTypes.push_back(actorType);
		getTargetActorsFromSector(actorTypes, output);
	}

	void Sector::getTargetActorsFromSector(const std::vector<ActorType>& actorTypes, std::unordered_set<ActorKey>& output) noexcept
	{
		ActorType actorType = ActorType::Count;
		const uint32 count = actorTypes.size();

		ScopedLock lock(this, true);
		std::unordered_set<ActorKey>::iterator it = _actors.begin();
		const std::unordered_set<ActorKey>::const_iterator end = _actors.end();

		for (; it != end; ++it)
		{
			CommonActor* targetActor = g_app->getActorManager()->getActor(*it, true);
			if (nullptr == targetActor)
			{
				TA_ASSERT_DEV(false, "비정상입니다.");
				continue;
			}

			actorType = targetActor->getActorType();
			for (uint32 index = 0; index < count; ++index)
			{
				if (actorType == actorTypes[index])
				{
					output.insert(*it);
					break;
				}
			}
		}
	}

	void Sector::getTargetActorsFromSector(const ActorType& actorType, std::vector<ActorKey>& output) noexcept
	{
		std::vector<ActorType> actorTypes;
		actorTypes.reserve(1);
		actorTypes.push_back(actorType);
		getTargetActorsFromSector(actorTypes, output);
	}

	void Sector::getTargetActorsFromSector(const std::vector<ActorType>& actorTypes, std::vector<ActorKey>& output) noexcept
	{
		ActorType actorType = ActorType::Count;
		const uint32 count = actorTypes.size();

		ScopedLock lock(this, true);
		std::unordered_set<ActorKey>::iterator it = _actors.begin();
		const std::unordered_set<ActorKey>::const_iterator end = _actors.end();

		for (; it != end; ++it)
		{
			CommonActor* targetActor = g_app->getActorManager()->getActor(*it, true);
			if (nullptr == targetActor)
			{
				TA_ASSERT_DEV(false, "비정상입니다.");
				continue;
			}

			actorType = targetActor->getActorType();
			for (uint32 index = 0; index < count; ++index)
			{
				if (actorType == actorTypes[index])
				{
					output.push_back(*it);
					break;
				}
			}
		}
	}

	void Sector::getTargetActorsAndOtherActorsFromSector(const ActorType& actorType
														 , std::unordered_set<ActorKey>& typeActors
														 , std::unordered_set<ActorKey>& otherTypeActors) noexcept
	{
		std::vector<ActorType> actorTypes;
		actorTypes.reserve(1);
		actorTypes.push_back(actorType);
		getTargetActorsAndOtherActorsFromSector(actorTypes, typeActors, otherTypeActors);
	}

	void Sector::getTargetActorsAndOtherActorsFromSector(const std::vector<ActorType>& actorTypes
														 , std::unordered_set<ActorKey>& typeActors
														 , std::unordered_set<ActorKey>& otherTypeActors) noexcept
	{
		ActorType actorType = ActorType::Count;
		const uint32 count = actorTypes.size();

		ScopedLock lock(this, true);
		std::unordered_set<ActorKey>::iterator it = _actors.begin();
		const std::unordered_set<ActorKey>::const_iterator end = _actors.end();

		for (; it != end; ++it)
		{
			CommonActor* targetActor = g_app->getActorManager()->getActor(*it, true);
			if (nullptr == targetActor)
			{
				TA_ASSERT_DEV(false, "비정상입니다.");
				continue;
			}

			actorType = targetActor->getActorType();
			uint32 index = 0;
			for (; index < count; ++index)
			{
				if (actorType == actorTypes[index])
				{
					typeActors.insert(*it);
					break;
				}
			}

			if (count == index)
			{
				otherTypeActors.insert(*it);
			}
		}
	}

	void Sector::getTargetActorsWithActorTypeFromSector(std::unordered_map<ActorKey, ActorType>& output) noexcept
	{
		ActorType actorType = ActorType::Count;

		ScopedLock lock(this, true);
		std::unordered_set<ActorKey>::iterator it = _actors.begin();
		const std::unordered_set<ActorKey>::const_iterator end = _actors.end();

		for (; it != end; ++it)
		{
			CommonActor* targetActor = g_app->getActorManager()->getActor(*it, true);
			if (nullptr == targetActor)
			{
				TA_ASSERT_DEV(false, "비정상입니다.");
				continue;
			}

			actorType = targetActor->getActorType();
			output.insert(std::pair(*it, actorType));
		}
	}

	uint32 Sector::getOwnedActorCountForServer_(const GroupGameDataKey& groupGameDataKey) const noexcept
	{
		return uint32();
	}

	bool Sector::checkOwnedActorExistenceForServer_(const GroupGameDataKey& groupGameDataKey) const noexcept
	{
		return false;
	}

	bool Sector::registerToOwnedActorsForServer_(const GroupGameDataKey& groupGameDataKey, const ActorKey& actorKey) noexcept
	{
		return false;
	}

	bool Sector::deregisterFromOwnedActorsForServer_(const GroupGameDataKey& groupGameDataKey, const ActorKey& actorKey) noexcept
	{
		return false;
	}

	bool Sector::startSectorEventForServer(const ContentParameter& parameter, uint32 sectorEventIndex, bool isBasicSectorEvent) noexcept
	{
		return false;
	}

    bool Sector::removeActor_(const ActorKey& actor) noexcept
    {
        std::unordered_set<ActorKey>::iterator target = _actors.find(actor);
        if (_actors.end() == target)
        {
            TA_ASSERT_DEV(false, "해당 액터 삭제에 실패했습니다. 코드버그인지 확인해보세요")
            return false;
        }

        _actors.erase(actor);

        return true;
    }

    bool Sector::addActor_(const ActorKey& actor) noexcept
    {
        std::pair<std::unordered_set<ActorKey>::iterator, bool> rv = _actors.insert(actor);
        if (false == rv.second)
        {
            TA_ASSERT_DEV(false, "새로운 액터 추가에 실패했습니다. 코드버그인지 확인해보세요")
            return false;
        }

        return true;
    }
}

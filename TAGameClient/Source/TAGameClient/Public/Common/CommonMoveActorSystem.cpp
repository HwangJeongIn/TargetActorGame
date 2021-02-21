#include "Common/CommonMoveActorSystem.h"
#include "Common/CommonMoveActorComponent.h"
#include "Common/CommonActor.h"
#include "Common/ScopedLock.h"
#include "Common/GetComponentAndSystem.h"
#include "Common/Sector.h"


namespace ta
{
	CommonMoveActorSystem::~CommonMoveActorSystem(void) noexcept
	{
	}

	const ActorSystemType CommonMoveActorSystem::getActorSystemType(void) noexcept
	{
		return ActorSystemType::MoveSystem;
	}

	bool CommonMoveActorSystem::processMoveActor(CommonActor* target, const Vector& newPos, const bool isForced) const noexcept
	{
		return true;
	}

	bool CommonMoveActorSystem::processMoveToTarget(CommonMoveActorComponent* myMove, CommonMoveActorComponent* targetMove) const noexcept
	{
		return true;
	}

	bool CommonMoveActorSystem::processMoveToRandomPoint(CommonMoveActorComponent* myMove) const noexcept
	{
		return true;
	}

	CommonMoveActorSystem::CommonMoveActorSystem(void) noexcept
		: ActorSystem(ActorSystemType::MoveSystem)
	{
	}

	bool CommonMoveActorSystem::moveActor(CommonActor* target
							  , const Vector& newPos
							  , const bool isForced /*= false*/
							  , std::unordered_set<SectorKey>* oldSectors /*= nullptr*/
							  , std::unordered_set<SectorKey>* newSectors /*= nullptr*/) const noexcept
	{
		CommonMoveActorComponent* actorMoveCom = GetActorComponent<CommonMoveActorComponent>(target);
		if (nullptr == actorMoveCom)
		{
			TA_ASSERT_DEV(false, "컴포넌트가 없습니다.")
			return false;
		}

		const SectorKey newSectorKey = GetSectorIndex(newPos);
		// 유효한 섹터에 위치하고 있지않다. 그냥 리턴하자
		if (false == newSectorKey.isValid())
		{
			TA_ASSERT_DEV(false, "섹터안에 있지 않습니다.")
			return false;
		}

		{
			ScopedLock componentLock(actorMoveCom);

			if (false == move_(actorMoveCom, newPos, isForced))
			{
				TA_ASSERT_DEV(false, "move 단계에서 실패했습니다.")
					return false;
			}

			std::unordered_set<SectorKey>& nearSectors = actorMoveCom->getNewNearSectors_();
			const SectorKey currentSectorKey = actorMoveCom->getCurrentSectorKey_();
			if (newSectorKey != currentSectorKey)
			{
				if (false == moveSector_(actorMoveCom, newSectorKey))
				{
					TA_ASSERT_DEV(false, "moveSector 단계에서 실패했습니다.")
						return false;
				}

				if (nullptr != oldSectors)
				{
					*oldSectors = nearSectors;
				}
				
				GetNearSectors(newSectorKey, nearSectors);

				if (nullptr != newSectors)
				{
					*newSectors = nearSectors;
				}
			}
		}

		return true;
	}

	ActorKey CommonMoveActorSystem::getTargetActorFromSector(CommonMoveActorComponent* myMove
															 , const ActorType& actorType
															 , const bool searchNearSectors) const noexcept
	{
		std::vector<ActorType> actorTypes;
		actorTypes.reserve(1);
		actorTypes.push_back(actorType);

		return getTargetActorFromSector(myMove, actorTypes, searchNearSectors);
	}

	ActorKey CommonMoveActorSystem::getTargetActorFromSector(CommonMoveActorComponent* myMove
															 , const std::vector<ActorType>& actorTypes
															 , const bool searchNearSectors) const noexcept
	{
		Sector* sector = nullptr;
		ActorKey targetActorKey;
		if (false == searchNearSectors)
		{
			SectorKey sectorKey;
			{
				ScopedLock componentLock(myMove);
				sectorKey = myMove->getCurrentSectorKey_();
			}

			sector = GetSector(sectorKey);
			if (nullptr == sector)
			{
				TA_ASSERT_DEV(false, "섹터가 비정상입니다.");
				return ActorKey();
			}

			targetActorKey = sector->getTargetActorFromSector(actorTypes);
		}
		else
		{
			std::unordered_set<SectorKey> nearSectors;
			SectorKey sectorKey;
			{
				ScopedLock componentLock(myMove);
				nearSectors = myMove->getNewNearSectors_();
				sectorKey = myMove->getCurrentSectorKey_();
			}

			std::unordered_set<SectorKey>::iterator it = nearSectors.begin();
			const std::unordered_set<SectorKey>::iterator end = nearSectors.end();
			for (; it != end; ++it)
			{
				sector = GetSector(sectorKey);
				if (nullptr == sector)
				{
					TA_ASSERT_DEV(false, "섹터가 비정상입니다.");
					continue;
				}

				targetActorKey = sector->getTargetActorFromSector(actorTypes);
				if (true == targetActorKey.isValid())
				{
					break;
				}
			}
		}

		return targetActorKey;
	}

	void CommonMoveActorSystem::getTargetActorsFromSector(CommonMoveActorComponent* myMove
														  , const std::vector<ActorType>& actorTypes
														  , std::vector<ActorKey>& output
														  , const bool searchNearSectors) const noexcept
	{
		output.clear();
		Sector* sector = nullptr;
		if (false == searchNearSectors)
		{
			SectorKey sectorKey;
			{
				ScopedLock componentLock(myMove);
				sectorKey = myMove->getCurrentSectorKey_();
			}

			sector = GetSector(sectorKey);
			if (nullptr == sector)
			{
				TA_ASSERT_DEV(false, "섹터가 비정상입니다.");
			}

			sector->getTargetActorsFromSector(actorTypes, output);
		}
		else
		{
			std::unordered_set<SectorKey> nearSectors;
			SectorKey sectorKey;
			{
				ScopedLock componentLock(myMove);
				nearSectors = myMove->getNewNearSectors_();
				sectorKey = myMove->getCurrentSectorKey_();
			}

			std::unordered_set<SectorKey>::iterator it = nearSectors.begin();
			const std::unordered_set<SectorKey>::iterator end = nearSectors.end();
			for (; it != end; ++it)
			{
				sector = GetSector(sectorKey);
				if (nullptr == sector)
				{
					TA_ASSERT_DEV(false, "섹터가 비정상입니다.");
					continue;
				}

				sector->getTargetActorsFromSector(actorTypes, output);
			}
		}
	}

	bool CommonMoveActorSystem::move_(CommonMoveActorComponent* target
									  , const Vector& newPos
									  , const bool isForced) const noexcept
	{
		if (false == isForced && false == checkCanMove_(target, newPos))
		{
			return false;
		}

		if (false == target->setCurrentPosition_(newPos))
		{
			return false;
		}

		return true;
	}

	bool CommonMoveActorSystem::moveSector_(CommonMoveActorComponent* target, const SectorKey& newSectorKey) const noexcept
	{
		SectorKey currentSectorKey = target->getCurrentSectorKey_();
		if (newSectorKey == currentSectorKey)
		{
			return false;
		}

		CommonActor* owner = target->getOwner();
		Sector* newSector = GetSector(newSectorKey);
		TA_ASSERT_DEV((nullptr != newSector), "코드버그입니다..");

		// 처음에 아무곳도 안갔을때..
		if (true == currentSectorKey.isValid())
		{
			Sector* oldSector = GetSector(currentSectorKey);
			TA_ASSERT_DEV((nullptr != oldSector), "코드버그입니다..")
			oldSector->exitSector(owner);
		}

		newSector->enterSector(owner);
		target->setCurrentSectorKey_(newSectorKey);
		
		return true;
	}

	bool CommonMoveActorSystem::checkCanMove_(CommonMoveActorComponent* moveCom, const Vector& newPos) const noexcept
	{
		const float distance = (moveCom->getCurrentPosition_() - newPos).sizeSquared();
		// 나중에 스피드 검증이랑 추가로 들어가야한다
		if ((MaxMoveDistance* MaxMoveDistance) < distance)
		{
			return false;
		}

		return true;
	}

}


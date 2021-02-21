#include "Server/ServerMoveActorSystem.h"
#include "Server/ServerApp.h"
#include "Server/ServerActorManager.h"
#include "Server/AllPacketServer.h"
#include "Server/ServerMoveActorComponent.h"
#include "Server/ServerAiActorComponent.h"
#include "Server/ServerAiActorSystem.h"
#include "Common/Sector.h"
#include "Common/ScopedLock.h"
#include "Common/CommonActor.h"
#include "Common/GetComponentAndSystem.h"


namespace ta
{
	ServerMoveActorSystem::ServerMoveActorSystem(void) noexcept
	{
	}

	ServerMoveActorSystem::~ServerMoveActorSystem(void) noexcept
	{
	}

	void ServerMoveActorSystem::update(const ActorSystemUpdateParameter& updateParameter) const noexcept
	{
	}

	bool ServerMoveActorSystem::processMoveActor(CommonActor* target
													, const Vector& newPos
													, const bool isForced) const noexcept
	{
		return moveActorAndNotify(target, newPos, isForced);
	}

	bool ServerMoveActorSystem::processMoveToTarget(CommonMoveActorComponent* myMove, CommonMoveActorComponent* targetMove) const noexcept
	{
		return moveToTarget(myMove, targetMove);
	}

	bool ServerMoveActorSystem::processMoveToRandomPoint(CommonMoveActorComponent* myMove) const noexcept
	{
		return moveToRandomPoint(myMove);
	}

	bool ServerMoveActorSystem::processPostMove(CommonActor* target, const Vector& newPos) const noexcept
	{
		// 일단 나면 나한테 패킷 먼저 쏴주자
		if (ActorType::Player == target->getActorType())
		{
			const ActorKey targetActorKey = target->getActorKey();
			SendMoveActorSTC(targetActorKey, targetActorKey, newPos);
		}

		return true;
	}

	bool ServerMoveActorSystem::processNearSectors(CommonActor* targetActor
												   , const std::unordered_set<SectorKey>& fromSectors
												   , const std::unordered_set<SectorKey>& toSectors) const noexcept
	{
		
		const ActorType targetActorType = targetActor->getActorType();
		const ActorKey targetActorKey = targetActor->getActorKey();

		// 일반 몬스터들도 사용해야한다.
		//if (ActorType::Player != targetActorType)
		//{
		//	TA_ASSERT_DEV(false, "비정상입니다.");
		//	return false;
		//}

		ServerMoveActorComponent* targetActorMoveCom = GetActorComponent<ServerMoveActorComponent>(targetActorKey);
		if (nullptr == targetActorMoveCom)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		// 여기서 oldViewList, newViewList 액터 타입까지 받아두고 사용하기로 한다. // 어차피 중간에 필요해서 받아야한다.
		std::unordered_map<ActorKey,ActorType> oldViewList;
		{
			std::unordered_set<ActorKey> tempOldViewList;
			{
				ScopedLock targetActorMoveComLock(targetActorMoveCom, true);
				tempOldViewList = targetActorMoveCom->getViewList_();
			}
			convertToActorKeyWithActorType(tempOldViewList, oldViewList);
		}
		oldViewList.erase(targetActorKey);

		std::unordered_map<ActorKey, ActorType> newViewList;
		{
			Sector* sector = nullptr;
			std::unordered_set<SectorKey>::iterator it = toSectors.begin();
			const std::unordered_set<SectorKey>::const_iterator end = toSectors.end();
			for (; end != it; ++it)
			{
				sector = GetSector(*it);
				if (nullptr == sector)
				{
					TA_ASSERT_DEV(false, "비정상입니다.");
					return false;
				}
				sector->getTargetActorsWithActorTypeFromSector(newViewList);
			}
		}
		newViewList.erase(targetActorKey);

		// Create Move 처리
		{
			std::unordered_map<ActorKey, ActorType>::iterator it = newViewList.begin();
			const std::unordered_map<ActorKey, ActorType>::const_iterator end = newViewList.end();
			for (; end != it; ++it)
			{
				if (oldViewList.end() == oldViewList.find(it->first)) // old 존재X new 존재O
				{
					// 내 입장에서 새로 들어온 액터들 => 나도 모르기 때문에 나한테 액터를 Create 해준다.

					// 일단 나한테 해당 액터 들어왔다고 알려준다. // 플레이어가 아닌친구들까지 뷰를 관리해줄필요가 없다
					if (ActorType::Player == targetActorType)
					{
						createActorInClient(targetActorMoveCom, it->first, it->second);
						if (ActorType::Player != it->second)
						{
							activateAiIfDisabled(it->first);
						}
					}
					else
					{
						// AI인 경우 새로들어온것이 플레이어인경우 + AI가 비활성화되어있는 경우 AI를 깨워준다.
						if (ActorType::Player == it->second)
						{
							activateAiIfDisabled(targetActorKey);
						}
					}
				}

				// old 존재X new 존재O or old new 존재O // new에 존재하면 해당 sector actor가 플레이어일때 move나 create 둘 중 하나다

				// 플레이어가 아닌친구들까지 뷰를 관리해줄필요가 없다
				if (ActorType::Player != it->second)
				{
					continue;
				}

				ServerMoveActorComponent* sectorActorMoveCom = GetActorComponent<ServerMoveActorComponent>(it->first);
				if (nullptr == sectorActorMoveCom)
				{
					TA_ASSERT_DEV(false, "비정상입니다.");
					continue;
				}

				// 상대방이 나를 알때 Move / 모를때 Create 해준다.
				bool targetExistInSectorActorViewList = false;
				{
					ScopedLock sectorActorMoveComLock(sectorActorMoveCom, true);
					targetExistInSectorActorViewList = sectorActorMoveCom->checkExistInViewList_(targetActorKey);
				}

				if (true == targetExistInSectorActorViewList) // 상대방이 나를 알때 Move
				{
					moveActorInClient(sectorActorMoveCom, targetActorKey);
				}
				else // 상대방이 나를 모를때 Create
				{
					createActorInClient(sectorActorMoveCom, targetActorKey, targetActorType);
				}
			}
		}
		
		// Destroy처리
		{
			std::unordered_map<ActorKey, ActorType>::iterator it = oldViewList.begin();
			const std::unordered_map<ActorKey, ActorType>::const_iterator end = oldViewList.end();
			for (; end != it; ++it)
			{
				if (newViewList.end() != newViewList.find(it->first)) // old 존재O new 존재O
				{
					continue;
				}

				// old 존재O new 존재X

				// 내 입장에서 빠져나온 액터들 => 나도 SectorActor Destroy / SectorActor입장에서도 나 있으면 Destroy // 플레이어일때만 적용
				// 일단 나한테 해당 액터 들어왔다고 알려준다. // 플레이어가 아닌친구들까지 뷰를 관리해줄필요가 없다
				if (ActorType::Player == targetActorType)
				{
					destroyActorInClient(targetActorMoveCom, it->first);
					if (ActorType::Player != it->second)
					{
						deactivateAiIfNotDisabled(it->first);
					}
				}
				else
				{
					if (ActorType::Player == it->second)
					{
						deactivateAiIfNotDisabled(targetActorKey);
					}
				}

				// 플레이어가 아닌친구들까지 뷰를 관리해줄필요가 없다
				if (ActorType::Player != it->second)
				{
					continue;
				}

				ServerMoveActorComponent* sectorActorMoveCom = GetActorComponent<ServerMoveActorComponent>(it->first);
				if (nullptr == sectorActorMoveCom)
				{
					TA_ASSERT_DEV(false, "비정상입니다.");
					continue;
				}

				// 상대방이 나를 알때 Destroy
				bool targetExistInSectorActorViewList = false;
				{
					ScopedLock sectorActorMoveComLock(sectorActorMoveCom, true);
					targetExistInSectorActorViewList = sectorActorMoveCom->checkExistInViewList_(targetActorKey);
				}

				if (true == targetExistInSectorActorViewList) // 상대방이 나를 알때 Destroy
				{
					destroyActorInClient(sectorActorMoveCom, targetActorKey);
				}
			}
		}

		return true;

#ifdef old_ver
		std::vector<SectorKey> addedSectors;
		std::vector<SectorKey> removedSectors;
		CompareAndGetDifferentSectors(fromSectors, toSectors, addedSectors, removedSectors);

		Sector* sector = nullptr;
		const ActorType targetActorActorType = targetActor->getActorType();
		const ActorKey targetActorKey = targetActor->getActorKey();

		// added sectors
		{
			const uint32 count = addedSectors.size();
			for (uint32 index = 0; index < count; ++index)
			{
				sector = GetSector(addedSectors[index]);
				if (nullptr == sector)
				{
					TA_ASSERT_DEV(false, "해당 섹터가 없습니다 코드버그입니다.")
						continue;
				}

				ScopedLock sectorLock(sector);

				const std::unordered_set<ActorKey>& sectorActors = sector->getActors_();

				std::unordered_set<ActorKey>::iterator it = sectorActors.begin();
				const std::unordered_set<ActorKey>::iterator end = sectorActors.end();
				CommonActor* sectorActor = nullptr;
				for (; it != end; ++it)
				{
					// 여기서 보내자마자 바로 언스폰되면?
					// => 어차피 서버에서 스폰 언스폰 관리하기 때문에 이 패킷부터 처리되고 그다음에 언스폰이 내려온다

					sectorActor = GetActorManager()->getActor(*it);

					// sector actors에게 알리기
					const ActorType sectorActorType = sectorActor->getActorType();
					if (ActorType::Player == sectorActorType)
					{
						createActorInClient(*it, targetActorKey, targetActorActorType);
					}

					// 나한테 알리기
					if(ActorType::Player == targetActorActorType)
					{
						createActorInClient(targetActorKey, *it, sectorActorType);
					}
				}
			}
		}

		// removed sectors
		{
			const uint32 count = removedSectors.size();
			for (uint32 index = 0; index < count; ++index)
			{
				sector = GetSector(removedSectors[index]);
				if (nullptr == sector)
				{
					TA_ASSERT_DEV(false, "해당 섹터가 없습니다 코드버그입니다.")
						continue;
				}

				ScopedLock sectorLock(sector);

				const std::unordered_set<ActorKey>& sectorActors = sector->getActors_();

				std::unordered_set<ActorKey>::iterator it = sectorActors.begin();
				const std::unordered_set<ActorKey>::iterator end = sectorActors.end();
				CommonActor* sectorActor = nullptr;
				for (; it != end; ++it)
				{
					sectorActor = GetActorManager()->getActor(*it);

					// sector actors에게 알리기
					const ActorType sectorActorType = sectorActor->getActorType();
					if (ActorType::Player == sectorActorType)
					{
						destroyActorInClient(*it, targetActorKey);
					}

					// 나한테 알리기
					if (ActorType::Player == targetActorActorType)
					{
						destroyActorInClient(targetActorKey , *it);
					}
				}
			}
		}

		return true;
#endif
	}

	bool ServerMoveActorSystem::respondMoveActor(CommonActor* target, const Vector& newPos) const noexcept
	{
		return moveActorAndNotify(target, newPos, false, false);
	}

	bool ServerMoveActorSystem::moveActorAndNotify(CommonActor* target
												   , const Vector& newPos
												   , const bool isForced /*= false*/
												   , const bool notifyToClient /*= true*/) const noexcept
	{
		TA_LOG_DEV("moveActorAndNotify => targetActorKey : %d, newPos = (%f,%f,%f)"
				   , target->getActorKey().getKeyValue()
				   , newPos._x
				   , newPos._y
				   , newPos._z);

		std::unordered_set<SectorKey> oldSectors;
		std::unordered_set<SectorKey> newSectors;
		if (false == moveActor(target, newPos, isForced, &oldSectors, &newSectors))
		{
			TA_ASSERT_DEV(false, "MoveActor 단계에서 실패했습니다.")
				return false;
		}

		// 서버고 플레이어면 해당플레이어에게 먼저 패킷을 보낸다.
		if (true == notifyToClient)
		{
			if (false == processPostMove(target, newPos))
			{
				TA_ASSERT_DEV(false, "processPostMove 단계에서 실패했습니다.")
					return false;
			}
		}

		if (false == newSectors.empty())
		{
			if (false == processNearSectors(target, oldSectors, newSectors))
			{
				TA_ASSERT_DEV(false, "processNearSectors 단계에서 실패했습니다.")
					return false;
			}
		}

		return true;
	}

	void ServerMoveActorSystem::moveToCurrentPosition(CommonActor* target) const noexcept
	{
		const ActorKey& targetActorKey = target->getActorKey();
		ServerMoveActorComponent* moveActorComponent = GetActorComponent<ServerMoveActorComponent>(targetActorKey);

		Vector returnPos;
		{
			ScopedLock componentLock(moveActorComponent, true);
			returnPos = moveActorComponent->getCurrentPosition_();
		}

		SendMoveActorSTC(targetActorKey, targetActorKey, returnPos);
	}

	bool ServerMoveActorSystem::moveToTarget(CommonMoveActorComponent* myMove, CommonMoveActorComponent* targetMove) const noexcept
	{
		// TODO : 지금은 해당 포지션으로 바로 초기화하고 해당 포지션으로 바로 내려주지만 way point list만든다음 하나씩 내려줘야한다.
		Vector targetPosition;
		{
			ScopedLock lock(targetMove, true);
			targetPosition = targetMove->getCurrentPosition_();
		}

		if (false == moveActorAndNotify(myMove->getOwner(), targetPosition, true))
		{
			TA_ASSERT_DEV(false, "비정상입니다.")
			return false;
		}
		return true;
	}

	bool ServerMoveActorSystem::moveToRandomPoint(CommonMoveActorComponent* myMove) const noexcept
	{
		// TODO : 지금은 해당 포지션으로 바로 초기화하고 해당 포지션으로 바로 내려주지만 way point list만든다음 하나씩 내려줘야한다.
		Vector targetPosition;
		{
			ScopedLock lock(myMove);
			// 그냥 한번에 초기화해준다..
			targetPosition += myMove->getCurrentPosition_() + Vector(1, 1, 0);
			myMove->setCurrentPosition_(targetPosition);
		}

		if (false == moveActorAndNotify(myMove->getOwner(), targetPosition, true))
		{
			TA_ASSERT_DEV(false, "비정상입니다.")
				return false;
		}
		return true;
	}
	
	void ServerMoveActorSystem::moveActorInClient(ServerMoveActorComponent* clientActorMoveCom, const ActorKey& targetActorKey) const noexcept
	{
		const ActorKey& clientActorKey = clientActorMoveCom->getOwnerActorKey();

		Vector position;
		{
			ScopedLock lock(clientActorMoveCom, true);
			position = clientActorMoveCom->getCurrentPosition_();
		}

		SendMoveActorSTC(clientActorKey, targetActorKey, position);
	}

	void ServerMoveActorSystem::createActorInClient(ServerMoveActorComponent* clientActorMoveCom
													, const ActorKey& targetActorKey
													, const ActorType& actorType) const noexcept
	{
		const ActorKey& clientActorKey = clientActorMoveCom->getOwnerActorKey();

		//CommonActor* targetActor = GetActorManager()->getActor(targetActorKey, true);
		//if (nullptr == targetActor)
		//{

		//}
		//ActorType actorType = ActorType::Count;
		//{
		//	ScopedLock lock(clientActorMoveCom);
		//	clientActorMoveCom->addToViewList_(targetActorKey);
		//}

		{
			ScopedLock lock(clientActorMoveCom);
			clientActorMoveCom->addToViewList_(targetActorKey);
		}

		// createActorInClient은 항상 moveActorInClient를 타고 들어오도록 설계 // 앞에서 ViewList체크 끝났다.
		SendCreateActorSTC(clientActorKey, static_cast<uint32>(actorType), targetActorKey);

		// 컴포넌트 정보들도 싸서 보내주자
		SendAllComponentDataToClient(clientActorKey, targetActorKey);
	}

	void ServerMoveActorSystem::destroyActorInClient(ServerMoveActorComponent* clientActorMoveCom
													 , const ActorKey& actorKeyToDestroy) const noexcept
	{
		const ActorKey& clientActorKey = clientActorMoveCom->getOwnerActorKey();

		{
			ScopedLock lock(clientActorMoveCom);
			clientActorMoveCom->removeFromViewList_(actorKeyToDestroy);
		}

		SendDestroyActorSTC(clientActorKey, actorKeyToDestroy);
	}

	void ServerMoveActorSystem::convertToActorKeyWithActorType(const std::unordered_set<ActorKey>& input, std::unordered_map<ActorKey, ActorType>& output) const noexcept
	{
		std::unordered_set<ActorKey>::iterator it = input.begin();
		const std::unordered_set<ActorKey>::const_iterator end = input.end();

		ActorType actorType = ActorType::Count;
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
	
	bool ServerMoveActorSystem::activateAiIfDisabled(const ActorKey& targetActorKey) const noexcept
	{
		// AI인 경우 새로들어온것이 플레이어인경우 + AI가 비활성화되어있는 경우 AI를 깨워준다.
		ServerAiActorComponent* targetAi = GetActorComponent<ServerAiActorComponent>(targetActorKey);
		ServerAiActorSystem* aiSystem = GetActorSystem<ServerAiActorSystem>();
		bool isDisabledAi = false;
		if (nullptr != targetAi)
		{
			ScopedLock targetAiLock(targetAi, true);
			isDisabledAi = (AiState::Disabled == targetAi->getCurrentAiState_());
		}

		if (true == isDisabledAi)
		{
			if (false == aiSystem->activateAi(targetAi))
			{
				TA_ASSERT_DEV(false, "비정상입니다.");
				return false;
			}
		}

		return true;
	}
	
	bool ServerMoveActorSystem::deactivateAiIfNotDisabled(const ActorKey& targetActorKey) const noexcept
	{
		// AI인 경우 새로들어온것이 플레이어인경우 + AI가 비활성화되어있는 경우 AI를 깨워준다.
		ServerAiActorComponent* targetAi = GetActorComponent<ServerAiActorComponent>(targetActorKey);
		ServerAiActorSystem* aiSystem = GetActorSystem<ServerAiActorSystem>();
		bool isDisabledAi = false;
		if (nullptr != targetAi)
		{
			ScopedLock targetAiLock(targetAi, true);
			isDisabledAi = (AiState::Disabled == targetAi->getCurrentAiState_());
		}

		if (false == isDisabledAi)
		{
			if (false == aiSystem->deactivateAi(targetAi))
			{
				TA_ASSERT_DEV(false, "비정상입니다.");
				return false;
			}
		}

		return true;
	}
}
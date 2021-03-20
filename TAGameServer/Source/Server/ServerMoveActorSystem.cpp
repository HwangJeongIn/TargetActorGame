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
#include "Common/Serializer.h"
#include "Common/FileLoader.h"
#include "Common/StringUtility.h"
#include "RecastNavigation/RecastNavigationSystemUtility.h"
#include "RecastNavigation/NavMeshPoint.h"
#include "RecastNavigation/NavMeshPath.h"


namespace ta
{
	ServerMoveActorSystem::ServerMoveActorSystem(void) noexcept
		: _defaultRecastExtent{ 50.0f, 250.0f, 50.0f }
		, _defaultExtent(50.0f, 50.0f, 250.0f)
		, _defaultMaxNodes(2048)
		, _defaultCostLimit(3.40282347e+38)
		, _detourNavMesh(nullptr)
	{
	}

	ServerMoveActorSystem::~ServerMoveActorSystem(void) noexcept
	{
	}

	bool ServerMoveActorSystem::initialize(void) noexcept
	{
		Serializer slR;
		slR.setModeFlag(SerializerMode::Read | SerializerMode::WriteLog);

		std::vector<fs::path> navigationMeshFiles;
		FileLoader::getFilePathsFromDirectory(NavigationMeshPath, navigationMeshFiles);
		if (0 == navigationMeshFiles.size())
		{
			TA_ASSERT_DEV(false, "비정상");
			return false;
		}

		const uint32 count = navigationMeshFiles.size();
		std::string extention;
		for (uint32 index = 0; index < count; ++index)
		{
			Extension(navigationMeshFiles[index].string(), extention);
			if (0 != extention.compare("rnm"))
			{
				continue;
			}
			if (false == slR.importFromFile(navigationMeshFiles[index]))
			{
				TA_ASSERT_DEV(false, "비정상");
				return false;
			}

			// 일단 하나밖에 없다 , 추가하려면 하자
			if (false == serializeNavigationMesh(slR, _detourNavMesh))
			{
				TA_ASSERT_DEV(false, "비정상");
				return false;
			}

#ifdef CAN_CREATE_LOG_FILE
			if (false == slR.exportLogData(navigationMeshFiles[index] += "Read"))
			{
				TA_ASSERT_DEV(false, "비정상");
				return false;
			}
#endif 
		}
		
		// test
		
		if(true)
		{
			NavMeshPath path;
			findPath(ActorKey(), Vector(-60, 260, 73), Vector(540, 260, 73), path);
		}

		return true;
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
		std::unordered_map<ActorKey, ActorType> oldViewList;
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
					if (ActorType::Player == targetActorActorType)
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
						destroyActorInClient(targetActorKey, *it);
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

		TA_LOG_DEV("actor<%d> move to (%.1f, %.1f, %.1f) / observer : actor<%d>"
				   , targetActorKey.getKeyValue()
				   , position._x
				   , position._y
				   , position._z
				   , clientActorKey.getKeyValue());

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

		TA_LOG_DEV("create actor<%d> to actor<%d>", targetActorKey.getKeyValue(), clientActorKey.getKeyValue());

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

		TA_LOG_DEV("destroy actor<%d> from actor<%d>", actorKeyToDestroy.getKeyValue(), clientActorKey.getKeyValue());
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

	bool ServerMoveActorSystem::findPath(const ActorKey& targetActorKey, const Vector& startPos, const Vector& endPos, NavMeshPath& path) noexcept
	{
		if (true == path.hasPathPoint())
		{
			path.clearPath();
		}

		if (nullptr == _detourNavMesh)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		//CommonActor* targetActor = GetActorManager()->getActor(targetActorKey);
		//if (nullptr == targetActor)
		//{
		//	TA_ASSERT_DEV(false, "비정상입니다.");
		//	return false;
		//}

		dtNavMeshQuery query;
		query.init(_detourNavMesh, _defaultMaxNodes, nullptr);

		Vector recastStartPos, recastEndPos;
		dtPolyRef startPoly, endPoly;
		const bool prepareClear = preparePathFinding(startPos, endPos,
													 query, _defaultQueryFilter,
													 recastStartPos, startPoly,
													 recastEndPos, endPoly);

		if (false == prepareClear)
		{
			TA_LOG_DEV("preparePathFinding 실패");
			return false;
		}

		dtQueryResult pathResult;
		const dtStatus findPathStatus = query.findPath(startPoly, endPoly,
													   &recastStartPos._x, &recastEndPos._x,
													   _defaultCostLimit, &_defaultQueryFilter, pathResult, 0);



		generatePath(findPathStatus, path, query, &_defaultQueryFilter,
					 startPoly, endPoly,
					 RecastVectorToTAVector(recastStartPos), RecastVectorToTAVector(recastEndPos),
					 recastStartPos, recastEndPos,
					 pathResult);



		//		if (dtStatusDetail(FindPathStatus, DT_PARTIAL_RESULT))
		//		{
		//			Path.SetIsPartial(true);
		//			// this means path finding algorithm reached the limit of InQueryFilter.GetMaxSearchNodes()
		//			// nodes in A* node pool. This can mean resulting path is way off.
		//			Path.SetSearchReachedLimit(dtStatusDetail(FindPathStatus, DT_OUT_OF_NODES));
		//		}
		//
		//#if ENABLE_VISUAL_LOG
		//		if (dtStatusDetail(FindPathStatus, DT_INVALID_CYCLE_PATH))
		//		{
		//			UE_VLOG(NavMeshOwner, LogNavigation, Error, TEXT("FPImplRecastNavMesh::FindPath resulted in a cyclic path!"));
		//			FVisualLogEntry* Entry = FVisualLogger::Get().GetLastEntryForObject(NavMeshOwner);
		//			if (Entry)
		//			{
		//				Path.DescribeSelfToVisLog(Entry);
		//			}
		//		}
		//#endif // ENABLE_VISUAL_LOG
		//
		//		Path.MarkReady();
		//
		//		return DTStatusToNavQueryResult(FindPathStatus);

		return true;
	}

	bool ServerMoveActorSystem::projectPointToNavMesh(const Vector& point, Vector& result) const noexcept
	{
		if (nullptr == _detourNavMesh)
		{
			return false;
		}

		dtNavMeshQuery query;
		query.init(_detourNavMesh, _defaultMaxNodes, nullptr);

		bool rv = false;
		float rcClosestPoint[3];
		const Vector rcPoint = TAVectorToRecastVector(point);
		//const Vector rcExtent = TAVectorToRecastVector(_defaultExtent);
		dtPolyRef polyRef;
		query.findNearestPoly2D(&rcPoint._x, &_defaultExtent._x, &_defaultQueryFilter, &polyRef, rcClosestPoint);

		if (polyRef > 0)
		{
			// one last step required due to recast's BVTree imprecision
			const Vector& closestPoint = RecastVectorToTAVector(rcClosestPoint);
			const Vector closestPointDelta = closestPoint - point;
			if (-_defaultExtent._x <= closestPointDelta._x && closestPointDelta._x <= _defaultExtent._x
				&& -_defaultExtent._y <= closestPointDelta._y && closestPointDelta._y <= _defaultExtent._y
				&& -_defaultExtent._z <= closestPointDelta._z && closestPointDelta._z <= _defaultExtent._z)
			{
				rv = true;
				result = closestPoint;
			}
		}
		

		return rv;
	}

	bool ServerMoveActorSystem::preparePathFinding(const Vector& startPos, const Vector& endPos,
												   const dtNavMeshQuery& query, const dtQueryFilter& queryFilter,
												   Vector& recastStartPos, dtPolyRef& startPoly,
												   Vector& recastEndPos, dtPolyRef& endPoly) const noexcept
	{
		TA_TEMP_DEV("필요에 따라 변경");
		//const Vector navExtent = NavMeshOwner->GetModifiedQueryExtent(NavMeshOwner->GetDefaultQueryExtent());
		//const float extent[3] = { 0.0f, 0.0f, 0.008f };
		const Vector recastStartToProject = TAVectorToRecastVector(startPos);
		const Vector recastEndToProject = TAVectorToRecastVector(endPos);

		startPoly = INVALID_NAVNODEREF;
		query.findNearestPoly(&recastStartToProject._x, &_defaultRecastExtent._x, &queryFilter, &startPoly, &recastStartPos._x);
		if (startPoly == INVALID_NAVNODEREF)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		endPoly = INVALID_NAVNODEREF;
		query.findNearestPoly(&recastEndToProject._x, &_defaultRecastExtent._x, &queryFilter, &endPoly, &recastEndPos._x);
		if (endPoly == INVALID_NAVNODEREF)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		return true;
	}

	bool ServerMoveActorSystem::generatePath(dtStatus findPathStatus, NavMeshPath& path,
											 const dtNavMeshQuery& query, const dtQueryFilter* queryFilter,
											 dtPolyRef startPoly, dtPolyRef endPoly,
											 const Vector& startPos, const Vector& endPos,
											 const Vector& recastStartPos, const Vector& recastEndPos,
											 dtQueryResult& pathResult) const noexcept
	{
		// check for special case, where path has not been found, and starting polygon
		// was the one closest to the target
		if (pathResult.size() == 1 && dtStatusDetail(findPathStatus, DT_PARTIAL_RESULT))
		{
			// in this case we find a point on starting polygon, that's closest to destination
			// and store it as path end
			Vector recastHandPlacedPathEnd;
			query.closestPointOnPolyBoundary(startPoly, &recastEndPos._x, &recastHandPlacedPathEnd._x);

			path.addPathPoint(RecastVectorToTAVector(recastStartPos), startPoly);
			path.addPathPoint(RecastVectorToTAVector(recastHandPlacedPathEnd), startPoly);

			path.addPathCorridor(pathResult.getRef(0));
			path.addPathCorridorCost(calcSegmentCostOnPoly(startPoly, queryFilter, recastHandPlacedPathEnd, recastStartPos));

			return true;
		}

		// note that for recast partial path is successful, while we treat it as failed, just marking it as partial
		if (true == dtStatusSucceed(findPathStatus))
		{
			// check if navlink poly at end of path is allowed
			TA_TEMP_DEV("나중에 필요하면 추가");
			int32 pathSize = pathResult.size();
			//if (pathSize > 1 /* && bAllowNavLinkAsPathEnd*/) 
			//{
			//	uint16 polyFlags = 0;
			//	_detourNavMesh->getPolyFlags(pathResult.getRef(pathSize - 1), &polyFlags);
			//
			//	if (polyFlags & ARecastNavMesh::GetNavLinkFlag())
			//	{
			//		pathSize--;
			//	}
			//}

			if (pathSize == 1)
			{
				// failsafe cost for single poly corridor
				path.addPathCorridorCost(calcSegmentCostOnPoly(startPoly, queryFilter, endPos, startPos));
			}
			else
			{
				for (int32 i = 0; i < pathSize; i++)
				{
					path.addPathCorridorCost(pathResult.getCost(i));
				}
			}

			// copy over corridor poly data
			for (int i = 0; i < pathSize; ++i)
			{
				path.addPathCorridor(pathResult.getRef(i));
			}

			//Path.OnPathCorridorUpdated();

			TA_TEMP_DEV("나중에 필요하면 추가");
			// if we're backtracking this is the time to reverse the path.
			//if (Filter->getIsBacktracking())
			//{
			//	// for a proper string-pulling of a backtracking path we need to
			//	// reverse the data right now.
			//	Path.Invert();
			//	Swap(StartPolyID, EndPolyID);
			//	Swap(startPos, endPos);
			//	Swap(RecastStartPos, RecastEndPos);
			//}

//#if STATS
//			if (dtStatusDetail(FindPathStatus, DT_OUT_OF_NODES))
//			{
//				INC_DWORD_STAT(STAT_Navigation_OutOfNodesPath);
//			}
//
//			if (dtStatusDetail(FindPathStatus, DT_PARTIAL_RESULT))
//			{
//				INC_DWORD_STAT(STAT_Navigation_PartialPath);
//			}
//#endif

			//if (Path.WantsStringPulling())
			{
				Vector finalEndPos = endPos;

				// if path is partial (path corridor doesn't contain EndPolyID), find new RecastEndPos on last poly in corridor
				if (dtStatusDetail(findPathStatus, DT_PARTIAL_RESULT))
				{
					dtPolyRef lastPoly = path.getPathCorridor().back();
					float newEndPoint[3];

					const dtStatus newEndPointStatus = query.closestPointOnPoly(lastPoly, &recastEndPos._x, newEndPoint);
					if (dtStatusSucceed(newEndPointStatus))
					{
						finalEndPos = RecastVectorToTAVector(newEndPoint);
					}
				}

				findStraightPathFromCorridor(startPos, finalEndPos, path.getPathCorridor(), path.getPath(), &(path.getCustomLinkIds()));
			}
			TA_TEMP_DEV("나중에 필요하면 추가");
			//else
			//{
			//	// make sure at least beginning and end of path are added
			//	new(Path.GetPathPoints()) FNavPathPoint(startPos, StartPolyID);
			//	new(Path.GetPathPoints()) FNavPathPoint(endPos, EndPolyID);
			//
			//	// collect all custom links Ids
			//	for (int32 Idx = 0; Idx < Path.PathCorridor.Num(); Idx++)
			//	{
			//		const dtOffMeshConnection* OffMeshCon = DetourNavMesh->getOffMeshConnectionByRef(Path.PathCorridor[Idx]);
			//		if (OffMeshCon)
			//		{
			//			Path.CustomLinkIds.Add(OffMeshCon->userId);
			//		}
			//	}
			//}

			TA_TEMP_DEV("나중에 필요하면 추가");
			//if (Path.WantsPathCorridor())
			//{
			//	TArray<FNavigationPortalEdge> PathCorridorEdges;
			//	GetEdgesForPathCorridorImpl(&Path.PathCorridor, &PathCorridorEdges, NavQuery);
			//	Path.SetPathCorridorEdges(PathCorridorEdges);
			//}
			return true;
		}
		return false;
	}

	bool ServerMoveActorSystem::findStraightPathFromCorridor(const Vector& startPos, const Vector& endPos,
															 const std::vector<dtPolyRef>& pathCorridor, std::vector<NavMeshPoint*>& pathPoints, std::vector<uint32>* customLinks) const noexcept
	{
		dtNavMeshQuery query;
		query.init(_detourNavMesh, _defaultMaxNodes);


		const Vector recastStartPos = TAVectorToRecastVector(startPos);
		const Vector recastEndPos = TAVectorToRecastVector(endPos);
		bool rv = false;

		dtQueryResult stringPullResult;
		const dtStatus stringPullStatus = query.findStraightPath(&recastStartPos._x, &recastEndPos._x,
																 &pathCorridor[0], pathCorridor.size(), stringPullResult, DT_STRAIGHTPATH_AREA_CROSSINGS);
		{
			const uint32 count = pathPoints.size();
			for (uint32 index = 0; index < count; ++index)
			{
				delete pathPoints[index];
			}
			pathPoints.clear();
		}

		if (true == dtStatusSucceed(stringPullStatus))
		{
			const int32 stringPullResultSize = stringPullResult.size();
			pathPoints.reserve(stringPullResultSize);
			// convert to desired format
			NavMeshPoint* curVert = nullptr;
			for (int32 vertIdx = 0; vertIdx < stringPullResultSize; ++vertIdx)
			{
				const float* curRecastVert = stringPullResult.getPos(vertIdx);

				NavMeshPoint* curVert = new NavMeshPoint(RecastVectorToTAVector(curRecastVert), stringPullResult.getRef(vertIdx));

				pathPoints.push_back(curVert);

				TA_TEMP_DEV("");
				//FNavMeshNodeFlags CurNodeFlags(0);
				//CurNodeFlags.PathFlags = StringPullResult.getFlag(VertIdx);
				//
				//uint8 AreaID = RECAST_DEFAULT_AREA;
				//DetourNavMesh->getPolyArea(CurVert->NodeRef, &AreaID);
				//CurNodeFlags.Area = AreaID;
				//
				//const UClass* AreaClass = NavMeshOwner->GetAreaClass(AreaID);
				//const UNavArea* DefArea = AreaClass ? ((UClass*)AreaClass)->GetDefaultObject<UNavArea>() : NULL;
				//CurNodeFlags.AreaFlags = DefArea ? DefArea->GetAreaFlags() : 0;
				//
				//CurVert->Flags = CurNodeFlags.Pack();
				//
				//// include smart link data
				//// if there will be more "edge types" we change this implementation to something more generic
				//if (customLinks && (CurNodeFlags.PathFlags & DT_STRAIGHTPATH_OFFMESH_CONNECTION))
				//{
				//	const dtOffMeshConnection* offMeshCon = DetourNavMesh->getOffMeshConnectionByRef(CurVert->NodeRef);
				//	if (offMeshCon)
				//	{
				//		CurVert->CustomLinkId = OffMeshCon->userId;
				//		CustomLinks->Add(OffMeshCon->userId);
				//	}
				//}
				//
				//CurVert++;
			}

			// findStraightPath returns 0 for polyId of last point for some reason, even though it knows the poly id.  We will fill that in correctly with the last poly id of the corridor.
			// @TODO shouldn't it be the same as EndPolyID? (nope, it could be partial path)
			pathPoints.back()->_polyRef = pathCorridor.back();
			rv = true;
		}

		return rv;
	}

	float ta::ServerMoveActorSystem::calcSegmentCostOnPoly(const dtPolyRef& PolyID, const dtQueryFilter* queryFilter, const Vector& startPos, const Vector& endPos) const noexcept
	{
		uint8 areaID = RECAST_DEFAULT_AREA;
		_detourNavMesh->getPolyArea(PolyID, &areaID);

		const float areaTravelCost = queryFilter->getAreaCost(areaID);
		return areaTravelCost * (endPos - startPos).size();
	}
}

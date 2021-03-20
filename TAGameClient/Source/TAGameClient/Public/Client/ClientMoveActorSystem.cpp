#include "Client/ClientMoveActorSystem.h"
#include "Client/AllPacketClient.h"
#include "Common/CommonActor.h"
#include "Common/CommonMoveActorComponent.h"
#include "Common/ScopedLock.h"

#ifndef TA_SERVER

#include "Common/Serializer.h"
#include "NavigationSystem.h"
#include "TAGameInstance.h"
#include "TAPlayerController.h"
#include "NavMesh/RecastNavMesh.h"

#endif


namespace ta
{
	ClientMoveActorSystem::ClientMoveActorSystem(void) noexcept
	{
	}

	ClientMoveActorSystem::~ClientMoveActorSystem(void) noexcept
	{
	}

	bool ClientMoveActorSystem::initialize(void) noexcept
	{
		return true;
	}

	void ClientMoveActorSystem::update(const ActorSystemUpdateParameter& updateParameter) const noexcept
	{
	}

#ifndef TA_SERVER
	bool ClientMoveActorSystem::exportRecastNavMesh(void) noexcept
	{
		UNavigationSystemV1* navSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetTAGameWorld());
		if (nullptr == navSys)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		ATAPlayerController* playerController = GetFirstTAPlayerController();
		if (nullptr == playerController)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		//FNavAgentProperties nagAgentProperties;
		//nagAgentProperties.bCanCrouch = 0;
		//nagAgentProperties.bCanJump = 1;
		//nagAgentProperties.bCanWalk = 1;
		//nagAgentProperties.bCanSwim = 1;
		//nagAgentProperties.bCanFly = 0;
		//
		//nagAgentProperties.AgentRadius = 34.0f;
		//nagAgentProperties.AgentHeight = 142.0f;
		//nagAgentProperties.AgentStepHeight = -1.0f;
		//nagAgentProperties.NavWalkingSearchHeightScale = 0.5f;

		ANavigationData* navData = (navSys == nullptr) ? nullptr : navSys->GetNavDataForProps(playerController->GetNavAgentPropertiesRef()
																							  , FVector::ZeroVector);

		ARecastNavMesh* recastNavMesh = Cast<ARecastNavMesh>(navData);
		if (nullptr == recastNavMesh)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		dtNavMesh* detourNavMesh = recastNavMesh->GetRecastMesh();
		if (nullptr == detourNavMesh)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		Serializer slW;
		slW.setModeFlag(SerializerMode::Write | SerializerMode::WriteLog);
		if (false == serializeNavigationMesh(slW, detourNavMesh))
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		fs::path finalPath = NavigationMeshPath / "RecastNavigationMesh.rnm";
		if (false == slW.exportToFile(finalPath))
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

#ifdef CAN_CREATE_LOG_FILE
		if (false == slW.exportLogData(finalPath += "Write"))
		{
			TA_ASSERT_DEV(false, "비정상");
			return false;
		}
#endif 

		return true;
	}
#endif

	bool ClientMoveActorSystem::processMoveActor(CommonActor* target, const Vector& newPos, const bool isForced) const noexcept
	{
		return requestMoveActor(target, newPos);
	}

	bool ClientMoveActorSystem::processMoveToTarget(CommonMoveActorComponent* myMove, CommonMoveActorComponent* targetMove) const noexcept
	{
		return requestMoveToTarget(myMove->getOwner(), targetMove->getOwner());
	}

	bool ClientMoveActorSystem::processMoveToRandomPoint(CommonMoveActorComponent* myMove) const noexcept
	{
		return requestMoveToRandomPoint(myMove->getOwner());
	}

	bool ClientMoveActorSystem::requestMoveActor(CommonActor* targetActor, const Vector& newPosition) const noexcept
	{
		if (nullptr == targetActor)
		{
			return false;
		}

		// 일단 targetActor가 네트워크가 있어야 보낼 수 있음
		// Player만 가능
		if (true == targetActor->hasNetwork())
		{
			const ActorKey& targetActorKey = targetActor->getActorKey();
			SendMoveActorCTS(targetActorKey, targetActorKey, newPosition);
		}

		return true;
	}

	bool ClientMoveActorSystem::requestMoveToTarget(CommonActor* myActor, CommonActor* targetActor) const noexcept
	{
		if (nullptr == myActor
			|| nullptr == targetActor)
		{
			return false;
		}

		if (false == myActor->isPlayer())
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}
		const ActorKey& myActorKey = myActor->getActorKey();
		SendMoveToTargetCTS(myActorKey, myActorKey, targetActor->getActorKey());
		return true;
	}

	bool ClientMoveActorSystem::requestMoveToRandomPoint(CommonActor* myActor) const noexcept
	{
		if (nullptr == myActor)
		{
			return false;
		}

		if (false == myActor->isPlayer())
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		const ActorKey& myActorKey = myActor->getActorKey();
		SendMoveToRandomPointCTS(myActorKey, myActorKey);
		return true;
	}

	bool ClientMoveActorSystem::respondMoveActor(CommonActor* target, const Vector& newPos) const noexcept
	{
		return moveActor(target, newPos, true);
	}
	
	bool ClientMoveActorSystem::respondRotateActor(CommonMoveActorComponent* myMove, const Vector& newDirection) const noexcept
	{
		ScopedLock lock(myMove);
		myMove->setCurrentRotation_(newDirection);
		return true;
	}
}
#include "Client/ClientMoveActorSystem.h"
#include "Client/AllPacketClient.h"
#include "Common/CommonActor.h"
#include "Common/CommonMoveActorComponent.h"
#include "Common/ScopedLock.h"
#include "Common/FileLoader.h"
#include "Common/StringUtility.h"



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
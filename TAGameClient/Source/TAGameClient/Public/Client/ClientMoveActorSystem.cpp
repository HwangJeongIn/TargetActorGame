#ifndef TA_SERVER
#include "TAGameEvent.h"
#endif

#include "Client/ClientMoveActorSystem.h"
#include "Client/AllPacketClient.h"
#include "Client/ClientActor.h"
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
		//TA_LOG_DEV("<RespondMoveActor> => ActorKey : %d, position(%.1f, %.1f, %.1f)", target->getActorKey().getKeyValue(), newPos._x, newPos._y, newPos._z);
		const bool rv = moveActor(target, newPos, true);
		if (false == rv)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

#ifndef TA_SERVER
		TAGameEventMoveToLocation* event = new TAGameEventMoveToLocation;
		event->setActorKey(target->getActorKey());
		event->setDestination(newPos);

		if (false == RegisterTAGameEvent(event))
		{
			TA_ASSERT_DEV(false, "이벤트 등록에 실패했습니다.");
		}
#endif

		return rv;
	}
	
	bool ClientMoveActorSystem::respondRotateActor(CommonMoveActorComponent* myMove, const Vector& newDirection) const noexcept
	{
		ScopedLock lock(myMove);
		myMove->setCurrentRotation_(newDirection);
		return true;
	}
	
	bool ClientMoveActorSystem::setStartTransform(CommonActor* target, const Vector& newPos, const Vector& newRot) const noexcept
	{
		const bool rv = moveActor(target, newPos, true);
		if (false == rv)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

#ifndef TA_SERVER
		//TAGameEventSetTransform* event = new TAGameEventSetTransform;
		//event->setActorKey(target->getActorKey());
		//event->setPosition(newPos);
		//event->setRotation(newRot);
		//
		//if (false == RegisterTAGameEvent(event))
		//{
		//	TA_ASSERT_DEV(false, "이벤트 등록에 실패했습니다.");
		//}
#endif
		return true;
	}
}
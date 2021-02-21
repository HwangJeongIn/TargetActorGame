#include "Client/ClientAiActorSystem.h"
#include "Client/ClientAiActorComponent.h"
#include "Client/AllPacketClient.h"
#include "Common/CommonActor.h"


namespace ta
{
	ClientAiActorSystem::ClientAiActorSystem(void) noexcept
	{
	}

	ClientAiActorSystem::~ClientAiActorSystem(void) noexcept
	{
	}

	bool ClientAiActorSystem::processDetectTargetPlayer(CommonAiActorComponent* myAi, CommonMoveActorComponent* myMove) const noexcept
	{
		return requestDetectTarget(myAi->getOwner(), ActorType::Player);
	}

	bool ClientAiActorSystem::processDetectTargetMonster(CommonAiActorComponent* myAi, CommonMoveActorComponent* myMove) const noexcept
	{
		return requestDetectTarget(myAi->getOwner(), ActorType::Monster);
	}

	bool ClientAiActorSystem::requestDetectTarget(CommonActor* myActor, const ActorType& targetActorType) const noexcept
	{
		if (nullptr == myActor)
		{
			return false;
		}

		if (false == myActor->isPlayer())
		{
			return false;
		}

		const ActorKey& myActorKey = myActor->getActorKey();
		SendDetectTargetCTS(myActorKey, myActorKey, static_cast<uint32>(targetActorType));
		return true;
	}

}
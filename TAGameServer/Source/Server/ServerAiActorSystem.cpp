#include "Server/ServerAiActorSystem.h"
#include "Server/ServerAiActorComponent.h"
#include "Server/AllPacketServer.h"
#include "Common/CommonMoveActorSystem.h"
#include "Common/GetComponentAndSystem.h"
#include "Common/ScopedLock.h"
#include "Common/CommonApp.h"
#include "Common/ActorEvent.h"


namespace ta
{
	ServerAiActorSystem::ServerAiActorSystem(void) noexcept
	{
	}

	ServerAiActorSystem::~ServerAiActorSystem(void) noexcept
	{
	}

	bool ServerAiActorSystem::processCommandTickAi(CommonActor* actor, const float deltaTime) const noexcept
	{
		ServerAiActorComponent* targetAi = GetActorComponent<ServerAiActorComponent>(actor);
		if (nullptr == targetAi)
		{
			//TA_ASSERT_DEV(false, "비정상입니다."); // 있을 수 있다.
			return false;
		}


		{
			ScopedLock aiComLock(targetAi);

			if (AiState::Disabled == targetAi->getCurrentAiState_())
			{
				TA_LOG_DEV("< Ai Disabled > Target : %d", actor->getActorKey().getKeyValue());
				return true;
			}

			targetAi->setCurrentAiState_(AiState::None);
		}

		// TA_LOG_DEV("< Ai Tick > Target : %d", actor->getActorKey().getKeyValue());

		// 락 안 걸고 사용해도된다. 접근하는 ai는 static변수로 세팅되어있는데, 
		// 액터가 스폰될때 이 static변수로 세팅된후, Component Valid가 true가 된다. 
		// Componet Valid가 true이면 항상 세팅되어있고, Componet Valid가 중간에 true => false로 변한다고해도 ai를 초기화하지않고 놔두기 때문에 
		// 그 사이에 새로운 액터가 생성되어 ai를 초기화하기 전까지는 안전하다. // 이짧은 순간에 액터까지 생성되고 ai건드린다면....
		const ActorKey targetActorKey = targetAi->getOwnerActorKey();
		{
			if (false == targetAi->getAi().tick(targetActorKey, deltaTime))
			{
				TA_ASSERT_DEV(false, "Ai Tick Failed!");
				return false;
			}
		}


		ActorEventObject* aiEvent = new ActorEventObject;
		aiEvent->_actorEventType = ActorEventType::AiEvent;
		aiEvent->_aiCommandType = AiCommandType::TickAi;
		aiEvent->_myActorKey = targetActorKey;

		if (false == RegisterActorEvent(aiEvent, AiTickInterval))
		{
			TA_ASSERT_DEV(false, "이벤트 등록에 실패했습니다.");
			return false;
		}

		return true;
	}

	bool ServerAiActorSystem::processDetectTargetPlayer(CommonAiActorComponent* myAi, CommonMoveActorComponent* myMove) const noexcept
	{
		return detectTarget(myAi, myMove, ActorType::Player);
	}

	bool ServerAiActorSystem::processDetectTargetMonster(CommonAiActorComponent* myAi, CommonMoveActorComponent* myMove) const noexcept
	{
		return detectTarget(myAi, myMove, ActorType::Monster);
	}
	
	bool ServerAiActorSystem::detectTarget(CommonAiActorComponent* myAi, CommonMoveActorComponent* myMove, const ActorType& actorType) const noexcept
	{
		CommonMoveActorSystem* moveSystem = GetActorSystem<CommonMoveActorSystem>();
		ActorKey targetActorKey = moveSystem->getTargetActorFromSector(myMove, actorType); // 일단 속한 섹터 찾는걸로 해두자..
		if (false == targetActorKey.isValid())
		{
			return false;
		}

		{
			ScopedLock aiComLock(myAi);
			static_cast<ServerAiActorComponent*>(myAi)->_targetActorKey = targetActorKey;
		}

		// 클라로 내려준다. // 자기자신만 알면된다. 다른 클라로 안내려줌
		CommonActor* myActor = myAi->getOwner();
		if (true == myActor->isPlayer())
		{
			const ActorKey& myActorKey = myActor->getActorKey();
			SendSetTargetActorSTC(myActorKey, myActorKey, targetActorKey);
		}

		return true;
	}

	bool ServerAiActorSystem::activateAi(ServerAiActorComponent* targetAi) const noexcept
	{
		{
			ScopedLock aiComLock(targetAi);

			if (AiState::Disabled != targetAi->getCurrentAiState_())
			{
				TA_ASSERT_DEV(false, "이미활성화되어있습니다.");
				return false;
			}

			targetAi->setCurrentAiState_(AiState::None);
		}

		ActorEventObject* aiEvent = new ActorEventObject;
		aiEvent->_actorEventType = ActorEventType::AiEvent;
		aiEvent->_aiCommandType = AiCommandType::TickAi;
		aiEvent->_myActorKey = targetAi->getOwnerActorKey();

		TA_LOG_DEV("Activate Ai , OwnerActorKey : %d", aiEvent->_myActorKey.getKeyValue());

		if (false == RegisterActorEvent(aiEvent, AiTickInterval))
		{
			TA_ASSERT_DEV(false, "이벤트 등록에 실패했습니다.");
			return false;
		}

		return true;
	}

	bool ServerAiActorSystem::deactivateAi(ServerAiActorComponent* targetAi) const noexcept
	{
		{
			ScopedLock aiComLock(targetAi);

			if (AiState::Disabled == targetAi->getCurrentAiState_())
			{
				TA_ASSERT_DEV(false, "이미비활성화되어있습니다.");
				return false;
			}

			targetAi->setCurrentAiState_(AiState::Disabled);
		}


		TA_LOG_DEV("Deactivate Ai , OwnerActorKey : %d", targetAi->getOwnerActorKey().getKeyValue());

		return true;
	}
}
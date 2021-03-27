#pragma once

#include "Common/CommonAiActorSystem.h"


namespace ta
{
	class ServerAiActorComponent;
}


namespace ta
{
	class ServerAiActorSystem : public CommonAiActorSystem
	{
	public:
		ServerAiActorSystem(void) noexcept;
		virtual ~ServerAiActorSystem(void) noexcept;

		virtual bool processCommandTickAi(CommonActor* actor, const float deltaTime) const noexcept override final;
		virtual bool processDetectTargetPlayer(CommonAiActorComponent* myAi, CommonMoveActorComponent* myMove) const noexcept override final;
		virtual bool processDetectTargetMonster(CommonAiActorComponent* myAi, CommonMoveActorComponent* myMove) const noexcept override final;
		virtual bool processMoveToPathPoint(CommonAiActorComponent* myAi, CommonMoveActorComponent* myMove, const float deltaTime) const noexcept override final;

		bool detectTarget(CommonAiActorComponent* myAi, CommonMoveActorComponent* myMove, const ActorType& actorType) const noexcept;

		bool activateAi(ServerAiActorComponent* targetAi) const noexcept;
		bool deactivateAi(ServerAiActorComponent* targetAi) const noexcept;
	};
}

#pragma once

#include "Common/CommonAiActorSystem.h"


namespace ta
{
	class ClientAiActorSystem : public CommonAiActorSystem
	{
	public:
		ClientAiActorSystem(void) noexcept;
		virtual ~ClientAiActorSystem(void) noexcept;

		virtual bool processDetectTargetPlayer(CommonAiActorComponent* myAi, CommonMoveActorComponent* myMove) const noexcept override final;
		virtual bool processDetectTargetMonster(CommonAiActorComponent* myAi, CommonMoveActorComponent* myMove) const noexcept override final;

		bool requestDetectTarget(CommonActor* myActor, const ActorType& targetActorType) const noexcept;
	};
}

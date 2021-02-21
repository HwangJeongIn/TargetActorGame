#pragma once

#include "Common/ActorSystem.h"
#include "Common/KeyDefinition.h"


namespace ta
{
	class CommonActor;
	class Vector;
	class CommonMoveActorComponent;
	class CommonAiActorComponent;
}


namespace ta
{
	class CommonAiActorSystem : public ActorSystem
	{
	public:
		virtual ~CommonAiActorSystem(void) noexcept;
		static const ActorSystemType getActorSystemType(void) noexcept;

		virtual bool processCommandTickAi(CommonActor* actor, const float deltaTime) const noexcept;
		virtual bool processDetectTargetPlayer(CommonAiActorComponent* myAi, CommonMoveActorComponent* myMove) const noexcept;
		virtual bool processDetectTargetMonster(CommonAiActorComponent* myAi, CommonMoveActorComponent* myMove) const noexcept;

		bool setTargetActor(CommonAiActorComponent* myAi, const ActorKey& targetActorKey) const noexcept;

	protected:
		CommonAiActorSystem(void) noexcept;

	};
}

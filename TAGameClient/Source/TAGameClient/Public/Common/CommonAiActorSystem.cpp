#include "Common/CommonAiActorSystem.h"
#include "Common/CommonAiActorComponent.h"
#include "Common/CommonActor.h"
#include "Common/ScopedLock.h"
#include "Common/GetComponentAndSystem.h"


namespace ta
{
	CommonAiActorSystem::~CommonAiActorSystem(void) noexcept
	{
	}

	const ActorSystemType CommonAiActorSystem::getActorSystemType(void) noexcept
	{
		return ActorSystemType::AiSystem;
	}

	bool CommonAiActorSystem::processCommandTickAi(CommonActor* actor, const float deltaTime) const noexcept
	{
		TA_ASSERT_DEV(false, "비정상입니다.");
		return false;
	}

	bool CommonAiActorSystem::processDetectTargetPlayer(CommonAiActorComponent* myAi, CommonMoveActorComponent* myMove) const noexcept
	{
		TA_ASSERT_DEV(false, "비정상입니다.");
		return false;
	}

	bool CommonAiActorSystem::processDetectTargetMonster(CommonAiActorComponent* myAi, CommonMoveActorComponent* myMove) const noexcept
	{
		TA_ASSERT_DEV(false, "비정상입니다.");
		return false;
	}

	bool CommonAiActorSystem::processMoveToPathPoint(CommonAiActorComponent* myAi, CommonMoveActorComponent* myMove, const float deltaTime) const noexcept
	{
		TA_ASSERT_DEV(false, "비정상입니다.");
		return false;
	}

	bool CommonAiActorSystem::setTargetActor(CommonAiActorComponent* myAi, const ActorKey& targetActorKey) const noexcept
	{
		if (false == targetActorKey.isValid())
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		ScopedLock lock(myAi);
		myAi->_targetActorKey = targetActorKey;

		return false;
	}

	CommonAiActorSystem::CommonAiActorSystem(void) noexcept
		: ActorSystem(ActorSystemType::AiSystem)
	{
	}
}


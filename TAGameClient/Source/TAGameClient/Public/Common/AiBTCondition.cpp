#include "Common/AiBTCondition.h"
#include "Common/CommonMoveActorComponent.h"
#include "Common/CommonAiActorComponent.h"
#include "Common/CommonMoveActorSystem.h"
#include "Common/GetComponentAndSystem.h"
#include "Common/ScopedLock.h"


namespace ta
{
	AiBTCondition::AiBTCondition(bool isNot) noexcept
		: _isNot(isNot)
	{
	}

	AiBTCondition::~AiBTCondition(void) noexcept
	{
	}

	bool AiBTCondition::checkCondition(const ActorKey& actorKey) const noexcept
	{
		bool rv = checkConditionDetail(actorKey);

		if (true == _isNot)
		{
			return !rv;
		}
		else
		{
			return rv;
		}
	}
}


namespace ta
{
	AiBTConditionHasTarget::AiBTConditionHasTarget(bool isNot) noexcept
		: AiBTCondition(isNot)
	{
	}

	AiBTConditionHasTarget::~AiBTConditionHasTarget(void) noexcept
	{
	}

	bool AiBTConditionHasTarget::checkConditionDetail(const ActorKey& actorKey) const noexcept
	{
		CommonAiActorComponent* aiCom = GetActorComponent<CommonAiActorComponent>(actorKey);
		if (nullptr == aiCom)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		bool rv = false;
		{
			ScopedLock componentLock(aiCom);
			rv = true == aiCom->_targetActorKey.isValid();
		}

		return rv;
	}
}


namespace ta
{
	AiBTConditionIsInAttackRange::AiBTConditionIsInAttackRange(bool isNot) noexcept
		: AiBTCondition(isNot)
	{
	}

	AiBTConditionIsInAttackRange::~AiBTConditionIsInAttackRange(void) noexcept
	{
	}

	bool AiBTConditionIsInAttackRange::checkConditionDetail(const ActorKey& actorKey) const noexcept
	{
		CommonAiActorComponent* aiCom = GetActorComponent<CommonAiActorComponent>(actorKey);
		if (nullptr == aiCom)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		CommonMoveActorComponent* moveCom = GetActorComponent<CommonMoveActorComponent>(actorKey);
		if (nullptr == moveCom)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		ActorKey targetActorKey;
		float attackRange = 0.0f;
		{
			ScopedLock componentLock(aiCom, true);
			targetActorKey = aiCom->_targetActorKey;
			attackRange = aiCom->_attackRange;
		}

		if (false == targetActorKey.isValid())
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		CommonMoveActorComponent* targetMoveCom = GetActorComponent<CommonMoveActorComponent>(targetActorKey);
		if (nullptr == targetMoveCom)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		Vector result;
		{
			{
				ScopedLock componentLock(moveCom, true);
				result = moveCom->getCurrentPosition_();
			}

			{
				ScopedLock targetComponentLock(targetMoveCom, true);
				result -= targetMoveCom->getCurrentPosition_();
			}
		}
		
		return result.sizeSquared() <= (attackRange * attackRange);
	}
}


namespace ta
{
	AiBTConditionHasPathPointPath::AiBTConditionHasPathPointPath(bool isNot) noexcept
		: AiBTCondition(isNot)
	{
	}

	AiBTConditionHasPathPointPath::~AiBTConditionHasPathPointPath(void) noexcept
	{
	}
	bool AiBTConditionHasPathPointPath::checkConditionDetail(const ActorKey& actorKey) const noexcept
	{
		CommonAiActorComponent* aiCom = GetActorComponent<CommonAiActorComponent>(actorKey);
		if (nullptr == aiCom)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		bool rv = false;
		{
			ScopedLock componentLock(aiCom, true);
			rv = aiCom->hasPathPointPath_();
		}

		return rv;
	}
}

#include "Common/CommonActionActorSystem.h"
#include "Common/ScopedLock.h"
#include "Common/CommonCharacterActorComponent.h"
#include "Common/GameData.h"


namespace ta
{
	CommonActionActorSystem::CommonActionActorSystem(void) noexcept
		: ActorSystem(ActorSystemType::ActionSystem)
	{
	}

	bool CommonActionActorSystem::calcAndChangeHp_(const float strength, CommonCharacterActorComponent* targetCom) const noexcept
	{
		// 공식 필요
		const bool rv = targetCom->setCurrentHp_(targetCom->getCurrentHp_() - strength);
		if (false == rv)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
		}

		return rv;
	}

	bool CommonActionActorSystem::changeHp_(const float hpValue, CommonCharacterActorComponent* targetCom) const noexcept
	{
		const bool rv = targetCom->setCurrentHp_(hpValue);
		if (false == rv)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
		}

		return rv;
	}

	CommonActionActorSystem::~CommonActionActorSystem(void) noexcept
	{
	}
	
	const ActorSystemType CommonActionActorSystem::getActorSystemType(void) noexcept
	{
		return ActorSystemType::ActionSystem;
	}

	bool CommonActionActorSystem::processAttackTarget(CommonCharacterActorComponent* myCom, CommonCharacterActorComponent* targetCom) const noexcept
	{
		return true;
	}
}


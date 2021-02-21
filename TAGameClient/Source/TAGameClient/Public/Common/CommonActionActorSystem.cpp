#include "Common/CommonActionActorSystem.h"
#include "Common/ScopedLock.h"
#include "Common/CommonCharacterActorComponent.h"


namespace ta
{
	CommonActionActorSystem::CommonActionActorSystem(void) noexcept
		: ActorSystem(ActorSystemType::ActionSystem)
	{
	}

	bool CommonActionActorSystem::calcAndChangeHp_(const uint32 strength, CommonCharacterActorComponent* targetCom) const noexcept
	{
		// 공식 필요
		targetCom->_currentHp -= strength;
		return true;
	}

	bool CommonActionActorSystem::changeHp_(const uint32 hpValue, CommonCharacterActorComponent* targetCom) const noexcept
	{
		if (targetCom->_maxHp < hpValue)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		targetCom->_currentHp = hpValue;
		return true;
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


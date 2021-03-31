#include "Client/ClientActionActorSystem.h"
#include "Common/CommonCharacterActorComponent.h"
#include "Common/ScopedLock.h"
#include "Common/GetComponentAndSystem.h"
#include "Client/ClientApp.h"
#include "Client/ClientActorManager.h"
#include "Client/AllPacketClient.h"


namespace ta
{
	ClientActionActorSystem::ClientActionActorSystem(void) noexcept
	{
	}

	ClientActionActorSystem::~ClientActionActorSystem(void) noexcept
	{
	}

	void ClientActionActorSystem::update(const ActorSystemUpdateParameter& updateParameter) const noexcept
	{
	}

	bool ClientActionActorSystem::processAttackTarget(CommonCharacterActorComponent* myCom, CommonCharacterActorComponent* targetCom) const noexcept
	{
		return requestAttackTarget(myCom->getOwnerActorKey(), targetCom->getOwnerActorKey());
	}

	bool ClientActionActorSystem::requestAttackTarget(const ActorKey& attacker, const ActorKey& targetActorKey) const noexcept
	{
		CommonActor* actor = GetActorManager()->getActor(attacker, true);
		if (ActorType::Player != actor->getActorType())
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		SendAttackTargetCTS(attacker, attacker, targetActorKey);
		return true;
	}

	bool ClientActionActorSystem::respondChangeHp(const ActorKey& attacker, const ActorKey& targetActorKey, const float& hpValue) const noexcept
	{
		// 추후 공격자 사용
		CommonCharacterActorComponent* targetCharacterCom = GetActorComponent<CommonCharacterActorComponent>(targetActorKey);
		if (nullptr == targetCharacterCom)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		{
			ScopedLock componentLock(targetCharacterCom);
			if (false == changeHp_(hpValue, targetCharacterCom))
			{
				TA_ASSERT_DEV(false, "비정상입니다.");
				return false;
			}
		}

		return true;
	}

	bool ClientActionActorSystem::respondKillActor(const ActorKey& targetActorKey) const noexcept
	{
		if (false == GetActorManager()->destroyActor(targetActorKey))
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		return true;
	}
}
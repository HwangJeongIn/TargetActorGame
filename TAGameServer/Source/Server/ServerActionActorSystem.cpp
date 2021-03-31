#include "Server/ServerActionActorSystem.h"
#include "Server/ServerActionActorSystem.h"
#include "Server/AllPacketServer.h"
#include "Common/KeyDefinition.h"
#include "Common/GetComponentAndSystem.h"
#include "Common/CommonCharacterActorComponent.h"
#include "Common/CommonMoveActorComponent.h"
#include "Common/CommonMoveActorSystem.h"
#include "Common/ScopedLock.h"
#include "Common/GameData.h"


namespace ta
{
	ServerActionActorSystem::ServerActionActorSystem(void) noexcept
	{
	}

	ServerActionActorSystem::~ServerActionActorSystem(void) noexcept
	{
	}

	void ServerActionActorSystem::update(const ActorSystemUpdateParameter& updateParameter) const noexcept
	{
	}

	bool ServerActionActorSystem::processAttackTarget(CommonCharacterActorComponent* myCom, CommonCharacterActorComponent* targetCom) const noexcept
	{
		return attackTarget(myCom, targetCom);
	}
	
	bool ServerActionActorSystem::attackTarget(CommonCharacterActorComponent* myCom, CommonCharacterActorComponent* targetCom) const noexcept
	{
		
		float myStrength = 0;
		{
			ScopedLock myLock(myCom);
			const CharacterGameData* characterGameData = myCom->getCharacterGameData_();
			if (nullptr == characterGameData)
			{
				TA_ASSERT_DEV(false, "비정상입니다.");
				return false;
			}

			myStrength = characterGameData->_strength;
		}

		float currentHp = 0;
		{
			ScopedLock targetLock(targetCom);

			if (false == calcAndChangeHp_(myStrength, targetCom))
			{
				return false;
			}

			currentHp = targetCom->getCurrentHp_();
		}

		bool isDead = false;
		if (currentHp <= 0)
		{
			isDead = true;
			if (false == GetActorManager()->destroyActor(targetCom->getOwner()))
			{
				TA_ASSERT_DEV(false, "비정상입니다.");
				return false;
			}
		}

		// 주변 액터들에게 알려준다.
		const ActorKey attackerActorKey = myCom->getOwnerActorKey();
		const ActorKey targetActorKey = targetCom->getOwnerActorKey();
		CommonMoveActorComponent* targetMove = GetActorComponent<CommonMoveActorComponent>(targetActorKey);

		CommonMoveActorSystem* moveSystem = GetActorSystem<CommonMoveActorSystem>();
		std::vector<ActorType> types; types.reserve(1); types.push_back(ActorType::Player);
		std::vector<ActorKey> output;
		moveSystem->getTargetActorsFromSector(targetMove, types, output, true);

		const uint32 count = output.size();
		for (uint32 index = 0; index < count; ++index)
		{
			if (false == isDead)
			{
				SendChangeHpSTC(output[index], attackerActorKey, targetActorKey, currentHp);
			}
			else
			{
				SendKillActorSTC(output[index], targetActorKey);
			}
		}

		return true;
	}
	
	bool ServerActionActorSystem::respondAttackTarget(const ActorKey& myActorKey, const ActorKey& targetActorKey) const noexcept
	{
		// 검증해야한다.
		CommonCharacterActorComponent* myCom = GetActorComponent<CommonCharacterActorComponent>(myActorKey);
		CommonCharacterActorComponent* targetCom = GetActorComponent<CommonCharacterActorComponent>(targetActorKey);
		
		if (nullptr == myCom
			|| nullptr == targetCom)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		if (false == attackTarget(myCom, targetCom))
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		return true;
	}
}
#include "Common/CommonCharacterActorSystem.h"
#include "Common/ScopedLock.h"
#include "Common/CommonCharacterActorComponent.h"
#include "Common/GameData.h"
#include "Common/GetComponentAndSystem.h"
#include "Common/BuffObject.h"
#include "Common/ScopedLock.h"
#include "Common/ContentEvent.h"
#include "Common/ContentParameter.h"


namespace ta
{
	CommonCharacterActorSystem::CommonCharacterActorSystem(void) noexcept
		: ActorSystem(ActorSystemType::CharacterSystem)
	{
	}

	CommonCharacterActorSystem::~CommonCharacterActorSystem(void) noexcept
	{
	}

	const ActorSystemType CommonCharacterActorSystem::getActorSystemType(void) noexcept
	{
		return ActorSystemType::CharacterSystem;
	}

	BuffObject* CommonCharacterActorSystem::checkAndRegisterBuff(CommonCharacterActorComponent* myCharacter
																 , const BuffGameDataKey& buffGameDataKey
																 , const ContentParameter& parameter) noexcept
	{
		if (false == buffGameDataKey.isValid())
		{
			TA_ASSERT_DEV(false, "비정상");
			return nullptr;
		}

		const BuffGameData* target = GetGameData<BuffGameData>(buffGameDataKey);
		if (nullptr == target)
		{
			TA_ASSERT_DEV(false, "비정상");
			return nullptr;
		}

		BuffObject* buffObject = nullptr;

		ScopedLock myCharacterLock(myCharacter);
		buffObject = myCharacter->getBuff_(buffGameDataKey);
		if (nullptr != buffObject)
		{
			return buffObject;
		}

		buffObject = new BuffObject;
		if (false == buffObject->initializeBuffObject(target))
		{
			TA_ASSERT_DEV(false, "비정상");
			delete buffObject;
			return nullptr;
		}

		if (false == target->isOneTime())
		{
			if (false == myCharacter->registerBuff_(buffGameDataKey, buffObject))
			{
				TA_ASSERT_DEV(false, "비정상");
				delete buffObject;
				return nullptr;
			}
		}

		return buffObject;
	}

	BuffObject* CommonCharacterActorSystem::checkAndUnregisterBuff(CommonCharacterActorComponent* myCharacter
																   , const BuffGameDataKey& buffGameDataKey
																   , const ContentParameter& parameter) noexcept
	{
		if (false == buffGameDataKey.isValid())
		{
			TA_ASSERT_DEV(false, "비정상");
			return nullptr;
		}

		BuffObject* buffObject = nullptr;
		{
			ScopedLock myCharacterLock(myCharacter);
			buffObject = myCharacter->getBuff_(buffGameDataKey);

			if (nullptr == buffObject)
			{
				TA_ASSERT_DEV(false, "등록되지 않은 버프를 해제하려합니다.");
				return nullptr;
			}

			if (false == myCharacter->unregisterBuff_(buffGameDataKey))
			{
				TA_ASSERT_DEV(false, "비정상");
				return nullptr;
			}
		}

		return buffObject;
	}

	bool CommonCharacterActorSystem::doBuff(CommonCharacterActorComponent* myCharacter, const BuffGameDataKey& buffGameDataKey, const ContentParameter& parameter) noexcept
	{
		return false;
	}

	bool CommonCharacterActorSystem::undoBuff(CommonCharacterActorComponent* myCharacter, const BuffGameDataKey& buffGameDataKey, const ContentParameter& parameter) noexcept
	{
		return false;
	}
	
	bool CommonCharacterActorSystem::addStat(CommonCharacterActorComponent* myCharacter, const CharacterStatType statType, const float point)
	{
		ScopedLock sectorLock(myCharacter);

		const float currentStat = myCharacter->getCurrentStat_(statType);
		if (false == myCharacter->setCurrentStat_(statType, currentStat + point))
		{
			TA_ASSERT_DEV(false, "비정상입니다");
			return false;
		}

		// 추가로 해야할 작업 있으면 수행

		return true;
	}
}


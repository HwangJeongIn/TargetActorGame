#include "Server/ServerCharacterActorSystem.h"
#include "Server/AllPacketServer.h"
#include "Common/CommonCharacterActorComponent.h"
#include "Common/GetComponentAndSystem.h"
#include "Common/BuffObject.h"
#include "Common/ScopedLock.h"
#include "Common/ContentEvent.h"
#include "Common/ContentParameter.h"
#include "Common/GameData.h"


namespace ta
{
	ServerCharacterActorSystem::ServerCharacterActorSystem(void) noexcept
	{
	}

	ServerCharacterActorSystem::~ServerCharacterActorSystem(void) noexcept
	{
	}

	void ServerCharacterActorSystem::update(const ActorSystemUpdateParameter& updateParameter) const noexcept
	{
	}

	bool ServerCharacterActorSystem::doBuff(CommonCharacterActorComponent* myCharacter
											, const BuffGameDataKey& buffGameDataKey
											, const ContentParameter& parameter) noexcept
	{
		BuffObject* buffObject = checkAndRegisterBuff(myCharacter, buffGameDataKey, parameter);
		if (nullptr == buffObject)
		{
			TA_ASSERT_DEV(false, "비정상");
			return false;
		}

		const BuffState buffState = buffObject->doBuff(parameter);

		// 클라로 패킷을 보내준다.
		const ActorKey myActorKey = myCharacter->getOwnerActorKey();
		SendDoBuffSTC(myActorKey, myCharacter->getOwnerActorKey(), buffGameDataKey, parameter);


		if (BuffState::Unsustainable == buffState)
		{
			const BuffGameData* buffGameData = GetGameData<BuffGameData>(buffGameDataKey);
			if (true == buffGameData->isOneTime())
			{
				delete buffObject;
				return true;
			}

			// 지속가능한 버프여서 등록해놨었지만 만료됐다. 빼준다.
			if (false == undoBuff(myCharacter, buffGameDataKey, parameter))
			{
				TA_ASSERT_DEV(false, "비정상");
				return false;
			}

			return true;
		}
		else
		{
			// 지속가능할 경우이벤트 등록
			ContentEventBuffObject* contentEventBuff = new ContentEventBuffObject;
			contentEventBuff->_contentEventType = ContentEventType::BuffEvent;
			contentEventBuff->_myActorKey = parameter._myActorKey;
			contentEventBuff->_targetActorKey = parameter._targetActorKey;
			contentEventBuff->_buffGameDataKey = buffGameDataKey;

			if (false == RegisterContentEvent(contentEventBuff, buffObject->getInterval()))
			{
				TA_ASSERT_DEV(false, "이벤트 등록에 실패했습니다.");
			}
		}

		return true;
	}

	bool ServerCharacterActorSystem::undoBuff(CommonCharacterActorComponent* myCharacter
											  , const BuffGameDataKey& buffGameDataKey
											  , const ContentParameter& parameter) noexcept
	{
		BuffObject* buffObject = checkAndUnregisterBuff(myCharacter, buffGameDataKey, parameter);
		if (nullptr == buffObject)
		{
			TA_ASSERT_DEV(false, "비정상");
			return false;
		}

		if (false == buffObject->undoBuff(parameter))
		{
			TA_ASSERT_DEV(false, "비정상");
			//return false;
		}

		delete buffObject;

		// 클라로 패킷을 보내준다.
		const ActorKey myActorKey = myCharacter->getOwnerActorKey();
		SendUndoBuffSTC(myActorKey, myCharacter->getOwnerActorKey(), buffGameDataKey, parameter);

		return true;
	}
}
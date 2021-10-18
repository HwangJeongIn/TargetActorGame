#include "Client/ClientCharacterActorSystem.h"
#include "Common/GetComponentAndSystem.h"
#include "Common/BuffObject.h"
#include "Common/ScopedLock.h"
#include "Common/ContentEvent.h"
#include "Common/ContentParameter.h"


namespace ta
{
	ClientCharacterActorSystem::ClientCharacterActorSystem(void) noexcept
	{
	}

	ClientCharacterActorSystem::~ClientCharacterActorSystem(void) noexcept
	{
	}

	void ClientCharacterActorSystem::update(const ActorSystemUpdateParameter& updateParameter) const noexcept
	{
	}
	
	bool ClientCharacterActorSystem::doBuff(CommonCharacterActorComponent* myCharacter
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

		if (BuffState::Unsustainable == buffState)
		{
			const BuffGameData* buffGameData = GetGameData<BuffGameData>(buffGameDataKey);
			if (true == buffGameData->isOneTime())
			{
				delete buffObject;
				return true;
			}

			//// 지속가능한 버프여서 등록해놨었지만 만료됐다. 빼준다. // 서버에서 처리한다.
			//if (false == undoBuff(myCharacter, buffGameDataKey, parameter))
			//{
			//	TA_ASSERT_DEV(false, "비정상");
			//	return false;
			//}

			//return true;
		}
		//else // 서버에서 처리
		//{
		//	// 지속가능할 경우이벤트 등록
		//	ContentEventBuffObject* contentEventBuff = new ContentEventBuffObject;
		//	contentEventBuff->_contentEventType = ContentEventType::BuffEvent;
		//	contentEventBuff->_myActorKey = parameter._myActorKey;
		//	contentEventBuff->_targetActorKey = parameter._targetActorKey;
		//	contentEventBuff->_buffGameDataKey = buffGameDataKey;

		//	if (false == RegisterContentEvent(contentEventBuff, buffObject->getInterval()))
		//	{
		//		TA_ASSERT_DEV(false, "이벤트 등록에 실패했습니다.");
		//	}
		//}

		return true;
	}
	
	bool ClientCharacterActorSystem::undoBuff(CommonCharacterActorComponent* myCharacter
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

		return true;
	}
}
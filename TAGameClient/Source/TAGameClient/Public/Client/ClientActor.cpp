#include "Client/ClientActor.h"
#include "Common/ScopedLock.h"

#ifndef TA_SERVER
#include "TACharacter.h"
#include "TAGameEvent.h"
#endif

namespace ta
{
	ClientActor::ClientActor(void) noexcept
#ifndef TA_SERVER
		: _unrealCharacter(nullptr)
#endif
	{
	}

	ClientActor::~ClientActor(void) noexcept
	{
	}

	void ClientActor::onToPool(void) noexcept
	{
		CommonActor::onToPool();

#ifndef TA_SERVER
		{
			ScopedLock actorLock(this);
			if (nullptr == _unrealCharacter)
			{
				TA_ASSERT_DEV(false, "이미 액터가 존재하지 않습니다. 비정상입니다.");
				return;
			}
		}

		TAGameEventDestroyActor* event = new TAGameEventDestroyActor;
		//event->_actorKey = getActorKey();
		if (false == RegisterTAGameEvent(event))
		{
			TA_ASSERT_DEV(false, "이벤트 등록에 실패했습니다.");
		}
#endif
	}

	void ClientActor::onFromPool(const CommonActorBasicSpawnData& spawnData) noexcept
	{
		CommonActor::onFromPool(spawnData);

#ifndef TA_SERVER
		_unrealCharacter = nullptr;
#endif
	}

	void ClientActor::onActive(void) noexcept
	{
#ifndef TA_SERVER
		ActorType actorType = ActorType::Count;
		{
			ScopedLock actorLock(this);
			if (nullptr != _unrealCharacter)
			{
				TA_ASSERT_DEV(false, "이미 액터가 존재합니다. 비정상입니다.");
				return;
			}

			actorType = getActorType_();
		}

		
		TAGameEventSpawnActor* event = new TAGameEventSpawnActor;
		event->setActorKey(getActorKey());
		if (ActorType::Player == actorType)
		{
			event->setMainPlayer(true);
		}

		if (false == RegisterTAGameEvent(event))
		{
			TA_ASSERT_DEV(false, "이벤트 등록에 실패했습니다.");
		}
#endif
		// 뒤에 해줘야 onActive시 관련된 정보를 받을 수 있다.
		CommonActor::onActive();
	}

#ifndef TA_SERVER
	ATACharacter* ClientActor::getUnrealCharacter_(void) noexcept
	{
		return _unrealCharacter;
	}

	bool ClientActor::setUnrealCharacter_(ATACharacter* character) noexcept
	{
		_unrealCharacter = character;
		return true;
	}
	
	bool ClientActor::destroyUnrealCharacter_(void) noexcept
	{
		if (false == _unrealCharacter->ConditionalBeginDestroy())
		{
			TA_ASSERT_DEV(false, "액터 삭제에 실패했습니다.");
			return false;
		}
		_unrealCharacter = nullptr;
		return true;
	}
#endif
}


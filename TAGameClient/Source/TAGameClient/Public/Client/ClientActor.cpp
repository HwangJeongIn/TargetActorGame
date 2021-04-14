#include "Client/ClientActor.h"
#include "Common/ScopedLock.h"

#ifndef TA_SERVER

#include "TAGameEvent.h"
#include "Actor.h"
#endif

namespace ta
{
	ClientActor::ClientActor(void) noexcept
#ifndef TA_SERVER
		: _unrealActor(nullptr)
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
			if (nullptr == _unrealActor)
			{
				TA_ASSERT_DEV(false, "이미 액터가 존재하지 않습니다. 비정상입니다.");
				return;
			}
		}

		TAGameEventDestroyActor* event = new TAGameEventDestroyActor;
		event->setActorKey(getActorKey());
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
		_unrealActor = nullptr;
#endif
	}

	void ClientActor::onActive(void) noexcept
	{
#ifndef TA_SERVER
		ActorType actorType = getActorType();
		{
			ScopedLock actorLock(this);
			if (_unrealActor)
			{
				TA_ASSERT_DEV(false, "이미 액터가 존재합니다. 비정상입니다.");
				return;
			}
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
	AActor* ClientActor::getUnrealActor_(void) noexcept
	{
		//if (!_unrealActor)
		//{
		//	return nullptr;
		//}
		//
		//return _unrealActor.Get();

		return _unrealActor;
	}

	bool ClientActor::setUnrealActor_(AActor* actor) noexcept
	{
		//_unrealActor = MakeShareable(character);
		_unrealActor = actor;
		return true;
	}
	
	bool ClientActor::resetUnrealActor_(void) noexcept
	{
		//if (!_unrealActor)
		//{
		//	TA_ASSERT_DEV(false, "액터 삭제에 실패했습니다.");
		//	return false;
		//}
		//
		//_unrealActor.Reset();
		//return true;
		_unrealActor = nullptr;
		return true;
	}
#endif
}


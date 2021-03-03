#include "TAGameEvent.h"
#include "TAGameInstance.h"
#include "TACharacter.h"
#include "TAConvertFunctions.h"
#include "Client/ClientApp.h"
#include "Client/ClientActorManager.h"
#include "Client/ClientActor.h"
#include "Client/ClientMoveActorComponent.h"
#include "Common/ScopedLock.h"
#include "Common/GetComponentAndSystem.h"



bool RegisterTAGameEvent(TAGameEvent* gameEvent) noexcept
{
	if (nullptr == gameEvent)
	{
		TA_ASSERT_DEV(false, "비정상입니다.");
		return false;
	}

	UTAGameInstance* gameInstance = GetTAGameInstance();
	if (nullptr == gameInstance)
	{
		TA_ASSERT_DEV(false, "비정상입니다.");
		return false;
	}

	return gameInstance->registerGameEvent(gameEvent);
}


TAGameEventProcessParameter::TAGameEventProcessParameter(void) noexcept
	: _gameInstance(nullptr)
	, _world(nullptr)
{
}

TAGameEventProcessParameter::~TAGameEventProcessParameter(void) noexcept
{
}



TAGameEvent::TAGameEvent(const TAGameEvent::GameEventType gameEventType) noexcept
	: _gameEventType(gameEventType)
{

}

TAGameEvent::~TAGameEvent(void) noexcept
{

}

TAGameEvent::GameEventType TAGameEvent::getGameEventType(void) const noexcept
{
	return _gameEventType;
}


TAGameEventSpawnActor::TAGameEventSpawnActor(void) noexcept
	: TAGameEvent(TAGameEvent::GameEventType::SpawnActor)
{

}

TAGameEventSpawnActor::~TAGameEventSpawnActor(void) noexcept
{

}

bool TAGameEventSpawnActor::processEvent(TAGameEventProcessParameter& parameter) noexcept
{
	if (nullptr == parameter._world)
	{
		TA_ASSERT_DEV(false, "게임월드가 존재하지 않습니다.");
		return false;
	}

	ta::ClientActor* actor = static_cast<ta::ClientActor*>(ta::GetActorManager()->getActor(_actorKey));
	if (nullptr == actor)
	{
		TA_ASSERT_DEV(false, "비정상입니다.");
		return false;
	}

	ta::ClientMoveActorComponent* actorMove = ta::GetActorComponent<ta::ClientMoveActorComponent>(_actorKey);
	if (nullptr == actorMove)
	{
		TA_ASSERT_DEV(false, "move 컴포넌트가 없습니다.");
		return false;
	}

	FVector position;
	{
		ta::ScopedLock componentLock(actorMove, true);
		ta::Vector currentPosition = actorMove->getCurrentPosition_();
		TA_LOG_DEV("actorkey : %d, current position (%d, %d, %d)", _actorKey.getKeyValue(), currentPosition._x, currentPosition._y, currentPosition._z);
		TAVectorToFVector(currentPosition, position);
	}
	
	{
		ta::ScopedLock actorLock(actor);
		//if (false == actor->isActive_())
		//{
		//	TA_ASSERT_DEV(false, "액터가 활성화 되어있지 않습니다. ActorKey : %d", _actorKey.getKeyValue());
		//	return false;
		//}

		//actor->getActorType_();
		FActorSpawnParameters actorSpawnParameters;
		actorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ATACharacter* character = parameter._world->SpawnActor<ATACharacter>(ATACharacter::StaticClass(), position, FRotator::ZeroRotator, SpawnCollisionHandlingOverride);
		if (nullptr == character)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		actor->setUnrealCharacter_(character);
	}

	return true;
}


TAGameEventDestroyActor::TAGameEventDestroyActor(void) noexcept
	: TAGameEvent(TAGameEvent::GameEventType::DestroyActor)
{

}

TAGameEventDestroyActor::~TAGameEventDestroyActor(void) noexcept
{

}

bool TAGameEventDestroyActor::processEvent(TAGameEventProcessParameter& parameter) noexcept
{
	if (nullptr == parameter._world)
	{
		TA_ASSERT_DEV(false, "게임월드가 존재하지 않습니다.");
		return false;
	}

	ta::ClientActor* actor = static_cast<ta::ClientActor*>(ta::GetActorManager()->getActor(_actorKey));
	if (nullptr == actor)
	{
		TA_ASSERT_DEV(false, "비정상입니다.");
		return false;
	}

	{
		ta::ScopedLock actorLock(actor);
		if (false == actor->isActive_())
		{
			TA_ASSERT_DEV(false, "액터가 활성화 되어있지 않습니다. ActorKey : %d", _actorKey.getKeyValue());
			return false;
		}

		if (false == actor->destroyUnrealCharacter_())
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}
	}

	return true;
}

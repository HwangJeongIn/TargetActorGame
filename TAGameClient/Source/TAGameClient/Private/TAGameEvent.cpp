#include "TAGameEvent.h"
#include "TAGameInstance.h"
#include "TAPlayer.h"
#include "TACharacter.h"
#include "TAConvertFunctions.h"
#include "TAGameInstance.h"
#include "TAPlayerController.h"
#include "TAInventoryUserWidget.h"
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

const ta::ActorKey TAGameEvent::getActorKey(void) const noexcept
{
	return _actorKey;
}

bool TAGameEvent::setActorKey(const ta::ActorKey& actorKey) noexcept
{
	if (false == actorKey.isValid())
	{
		TA_ASSERT_DEV(false, "비정상");
		return false;
	}
	
	_actorKey = actorKey;
	return true;
}

TAGameEventSpawnActor::TAGameEventSpawnActor(void) noexcept
	: TAGameEvent(TAGameEvent::GameEventType::SpawnActor)
	, _isMainPlayer(false)
{

}

TAGameEventSpawnActor::~TAGameEventSpawnActor(void) noexcept
{

}

void TAGameEventSpawnActor::setMainPlayer(const bool flag) noexcept
{
	_isMainPlayer = flag;
}

bool TAGameEventSpawnActor::processEvent(TAGameEventProcessParameter& parameter) noexcept
{
	if (nullptr == parameter._world)
	{
		TA_ASSERT_DEV(false, "게임월드가 존재하지 않습니다.");
		return false;
	}

	const ta::ActorKey actorKey = getActorKey();
	ta::ClientActor* actor = static_cast<ta::ClientActor*>(ta::GetActorManager()->getActor(actorKey));
	if (nullptr == actor)
	{
		TA_ASSERT_DEV(false, "비정상입니다.");
		return false;
	}

	ta::ClientMoveActorComponent* actorMove = ta::GetActorComponent<ta::ClientMoveActorComponent>(actorKey);
	if (nullptr == actorMove)
	{
		TA_ASSERT_DEV(false, "move 컴포넌트가 없습니다.");
		return false;
	}

	ATACharacter* character = nullptr;
	if (false == _isMainPlayer)
	{
		FVector position;
		{
			ta::ScopedLock componentLock(actorMove, true);
			ta::Vector currentPosition = actorMove->getCurrentPosition_();
			TA_LOG_DEV("<SpawnActor> => actorkey : %d, current position (%.1f, %.1f, %.1f)", actorKey.getKeyValue(), currentPosition._x, currentPosition._y, currentPosition._z);
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
			character = parameter._world->SpawnActor<ATACharacter>(ATACharacter::StaticClass(), position, FRotator::ZeroRotator, actorSpawnParameters);
			if (nullptr == character)
			{
				TA_ASSERT_DEV(false, "비정상입니다.");
				return false;
			}

		}
	}
	else
	{
		character = Cast<ATACharacter>(GetFirstTAPlayer());
		if (nullptr == character)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}
	}

	character->setActorKey(actorKey);
	actor->setUnrealCharacter_(character);

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

	const ta::ActorKey actorKey = getActorKey();
	ta::ClientActor* actor = static_cast<ta::ClientActor*>(ta::GetActorManager()->getActor(actorKey));
	if (nullptr == actor)
	{
		TA_ASSERT_DEV(false, "비정상입니다.");
		return false;
	}

	{
		ta::ScopedLock actorLock(actor);
		if (false == actor->isActive_())
		{
			TA_ASSERT_DEV(false, "액터가 활성화 되어있지 않습니다. ActorKey : %d", actorKey.getKeyValue());
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


TAGameEventInitializeInventory::TAGameEventInitializeInventory(void) noexcept
	: TAGameEvent(TAGameEvent::GameEventType::InitializeInventory)
	, _capacity(0)
{

}

TAGameEventInitializeInventory::~TAGameEventInitializeInventory(void) noexcept
{

}

bool TAGameEventInitializeInventory::processEvent(TAGameEventProcessParameter& parameter) noexcept
{
	if (nullptr == parameter._world)
	{
		TA_ASSERT_DEV(false, "게임월드가 존재하지 않습니다.");
		return false;
	}

	const ta::ActorKey actorKey = getActorKey();
	ta::ClientActor* actor = static_cast<ta::ClientActor*>(ta::GetActorManager()->getActor(actorKey));
	if (nullptr == actor)
	{
		TA_ASSERT_DEV(false, "비정상입니다.");
		return false;
	}
	
	ATAPlayerController* playerController = GetFirstTAPlayerController();
	if (nullptr == playerController)
	{
		TA_ASSERT_DEV(false, "비정상입니다.");
		return false;
	}

	UTAInventoryUserWidget* inventoryWidget = playerController->getInventoryUserWidget();
	
	TA_LOG_DEV("inventory capacity : %d", _capacity);
	inventoryWidget->setInventorySlotCount(_capacity);
	inventoryWidget->refreshSlots(actorKey);

	return true;
}

bool TAGameEventInitializeInventory::setCapacity(const ta::int32 capacity) noexcept
{
	if (static_cast<ta::int32>(ta::MaxInventoryCapacity) < capacity)
	{
		TA_ASSERT_DEV(false, "비정상입니다.");
		return false;
	}

	_capacity = capacity;
	return true;
}


TAGameEventRefreshInventory::TAGameEventRefreshInventory(void) noexcept
	: TAGameEvent(TAGameEvent::GameEventType::RefreshInventory)
	, _slotNo(-1)
{

}

TAGameEventRefreshInventory::~TAGameEventRefreshInventory(void) noexcept
{

}

bool TAGameEventRefreshInventory::processEvent(TAGameEventProcessParameter& parameter) noexcept
{
	if (nullptr == parameter._world)
	{
		TA_ASSERT_DEV(false, "게임월드가 존재하지 않습니다.");
		return false;
	}

	const ta::ActorKey actorKey = getActorKey();
	ta::ClientActor* actor = static_cast<ta::ClientActor*>(ta::GetActorManager()->getActor(actorKey));
	if (nullptr == actor)
	{
		TA_ASSERT_DEV(false, "비정상입니다.");
		return false;
	}

	ATAPlayerController* playerController = GetFirstTAPlayerController();
	if (nullptr == playerController)
	{
		TA_ASSERT_DEV(false, "비정상입니다.");
		return false;
	}

	UTAInventoryUserWidget* inventoryWidget = playerController->getInventoryUserWidget();
	if (false == inventoryWidget->refreshSlot(actorKey, _slotNo))
	{
		TA_ASSERT_DEV(false, "비정상입니다.");
		return false;
	}

	return true;
}

bool TAGameEventRefreshInventory::setSlotNo(const ta::ItemSlotNo slotNo) noexcept
{
	if (slotNo < 0)
	{
		return false;
	}

	_slotNo = slotNo;
	return true;
}

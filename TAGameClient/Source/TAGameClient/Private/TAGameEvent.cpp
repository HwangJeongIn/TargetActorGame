#include "TAGameEvent.h"
#include "TAGameInstance.h"
#include "TAPlayer.h"
#include "TACharacter.h"
#include "TANonPlayer.h"
#include "TAConvertFunctions.h"
#include "TAGameInstance.h"
#include "TAPlayerController.h"
#include "TAAIController.h"
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

	UTAGameInstance* gameInstance = TAGetGameInstance();
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

//=============================================================================================================
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
	FVector position;
	FRotator rotation;
	float speed;
	{
		ta::ScopedLock componentLock(actorMove, true);
		ta::Vector currentPosition = actorMove->getCurrentPosition_();
		ta::Vector currentRotation = actorMove->getCurrentRotation_();
		speed= actorMove->getSpeed_();

		TAVectorToFVector(currentPosition, position);
		rotation.Roll	= currentRotation._x;
		rotation.Pitch	= currentRotation._y;
		rotation.Yaw	= currentRotation._z;
	}

	if (false == _isMainPlayer)
	{
		character = TASpawnTAActor(actorKey, position, rotation);
		if (nullptr == character)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}
	}
	else
	{
		character = Cast<ATACharacter>(TAGetFirstPlayer());
		if (nullptr == character)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}
		character->SetActorLocation(position);
		character->SetActorRotation(rotation);

		character->setActorKey(actorKey);

		{
			ta::ScopedLock actorLock(actor);
			actor->setUnrealCharacter_(character);
		}

	}

	UCharacterMovementComponent* characterMovement = character->GetCharacterMovement();
	if (nullptr == characterMovement)
	{
		return false;
	}

	characterMovement->MaxWalkSpeed = speed;

	TA_LOG_DEV("<SpawnActor> => actorkey : %d, position : (%.1f, %.1f, %.1f), rotation : (%.1f, %.1f, %.1f), speed : %.1f", actorKey.getKeyValue()
			   , position.X, position.Y, position.Z
			   , rotation.Roll, rotation.Pitch, rotation.Yaw, speed);

	return true;
}


//=============================================================================================================
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

	if (false == TADestroyTAActor(actorKey))
	{
		TA_ASSERT_DEV(false, "비정상입니다.");
		return false;
	}

	return true;
}


//=============================================================================================================
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
	
	ATAPlayerController* playerController = TAGetFirstPlayerController();
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


//=============================================================================================================
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

	ATAPlayerController* playerController = TAGetFirstPlayerController();
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


//=============================================================================================================
TAGameEventMoveToLocation::TAGameEventMoveToLocation(void) noexcept
	: TAGameEvent(TAGameEvent::GameEventType::MoveToLocation)
{

}

TAGameEventMoveToLocation::~TAGameEventMoveToLocation(void) noexcept
{

}

bool TAGameEventMoveToLocation::processEvent(TAGameEventProcessParameter& parameter) noexcept
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

	ATAPlayerController* playerController = TAGetFirstPlayerController();
	if (nullptr == playerController)
	{
		TA_ASSERT_DEV(false, "비정상입니다.");
		return false;
	}

	{
		ATACharacter* character = actor->getUnrealCharacter_();
		if (nullptr == character)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		ATAPlayerController* playerController = Cast<ATAPlayerController>(character->GetController());
		if (nullptr != playerController)
		{
			// 플레이어 자동이동 구현해야됨
			return true;
		}

		ATAAIController* aiController = Cast<ATAAIController>(character->GetController());
		if (nullptr != aiController)
		{
			// 서버에서 길찾기 했기 떄문에 따로 길찾기 하지않는다.
			FVector destination;
			TAVectorToFVector(_destination, destination);
			//character->SetActorLocation(destination);
			aiController->MoveToLocation(destination, -1.0f, true, false);
			TA_LOG_DEV("<MoveToLocation> => actorkey : %d, current position (%.1f, %.1f, %.1f)", actorKey.getKeyValue(), _destination._x, _destination._y, _destination._z);

			return true;
		}

	}

	return false;
}

void TAGameEventMoveToLocation::setDestination(const ta::Vector& destination) noexcept
{
	_destination = destination;
}


//=============================================================================================================
TAGameEventSetTransform::TAGameEventSetTransform(void) noexcept
	: TAGameEvent(TAGameEvent::GameEventType::SetTransform)
{

}

TAGameEventSetTransform::~TAGameEventSetTransform(void) noexcept
{

}

bool TAGameEventSetTransform::processEvent(TAGameEventProcessParameter& parameter) noexcept
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

	TA_LOG_DEV("<SetTransform> => actorkey : %d, current position (%.1f, %.1f, %.1f)", actorKey.getKeyValue(), _position._x, _position._y, _position._z);
	{
		ATACharacter* character = actor->getUnrealCharacter_();
		if (nullptr == character)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		FVector position;
		TAVectorToFVector(_position, position);

		FRotator rotation;
		rotation.Roll = _rotation._x;
		rotation.Pitch = _rotation._y;
		rotation.Yaw = _rotation._z;

		character->SetActorLocation(position);
		character->SetActorRotation(rotation);
	}

	return true;
}

void TAGameEventSetTransform::setPosition(const ta::Vector& position) noexcept
{
	_position = position;
}

void TAGameEventSetTransform::setRotation(const ta::Vector& rotation) noexcept
{
	_rotation = rotation;
}

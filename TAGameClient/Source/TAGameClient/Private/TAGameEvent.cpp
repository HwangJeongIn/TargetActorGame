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
#include "TAObject.h"
#include "Client/ClientApp.h"
#include "Client/ClientActorManager.h"
#include "Client/ClientActor.h"
#include "Client/ClientMoveActorComponent.h"
#include "Client/ClientCharacterActorComponent.h"
#include "Client/ClientObjectActorComponent.h"
#include "Common/ScopedLock.h"
#include "Common/GetComponentAndSystem.h"
#include "Common/GameData.h"

//#define TA_MOVELOG


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
	ta::ClientActor* clientActor = static_cast<ta::ClientActor*>(ta::GetActorManager()->getActor(actorKey));
	if (nullptr == clientActor)
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

	AActor* aActor = nullptr;
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

	TA_TEMP_DEV("임시 수정해야됨");
	// 읽어와야한다.
	position.Z += 88.0f;

	if (false == _isMainPlayer)
	{
		aActor = TASpawnTAActor(actorKey, position, rotation);
		if (nullptr == aActor)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}
	}
	else
	{
		ATAPlayer* player = (TAGetFirstPlayer());
		if (nullptr == aActor)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}
		player->SetActorLocation(position);
		player->SetActorRotation(rotation);

		TAActor* taActor = static_cast<TAActor*>(player);
		if (nullptr == taActor)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		taActor->setActorKey(actorKey);
		{
			ta::ScopedLock actorLock(clientActor);
			clientActor->setUnrealActor_(player);
		}

		aActor = player;
	}

	const ta::ActorType actorType = clientActor->getActorType();
	if (ta::ActorType::Object != actorType) // 캐릭터용 액터
	{
		ATACharacter* character = Cast<ATACharacter>(aActor);
		if (nullptr == character)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		UCharacterMovementComponent* characterMovement = character->GetCharacterMovement();
		if (nullptr == characterMovement)
		{
			return false;
		}

		TA_TEMP_DEV("추후 수정해야함");
		characterMovement->MaxWalkSpeed = speed * 0.9f;

		ta::ClientCharacterActorComponent* characterCom = ta::GetActorComponent<ta::ClientCharacterActorComponent>(actorKey);
		if (nullptr == characterCom)
		{
			TA_ASSERT_DEV(false, "character 컴포넌트가 없습니다.");
			return false;
		}

		const ta::RenderingGameData* renderingGameData = characterCom->getRenderingGameData_();
		if (nullptr == renderingGameData)
		{
			TA_ASSERT_DEV(false, "RenderingGameData가 없습니다.");
			return false;
		}

		if (ta::MeshType::Skeletal != renderingGameData->_meshType)
		{
			TA_ASSERT_DEV(false, "캐릭터액터인데 SkeletalMesh가 아닙니다.");
			return false;
		}

		character->setSkeletalMeshAndAnimInstance(renderingGameData->_meshPath.c_str(), renderingGameData->_animInstancePath.c_str());
	}
	else // 오브젝트용 액터
	{
		ATAObject* object = Cast<ATAObject>(aActor);
		if (nullptr == object)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		ta::ClientObjectActorComponent* objectCom = ta::GetActorComponent<ta::ClientObjectActorComponent>(actorKey);
		if (nullptr == objectCom)
		{
			TA_ASSERT_DEV(false, "object 컴포넌트가 없습니다.");
			return false;
		}

		const ta::RenderingGameData* renderingGameData = objectCom->getRenderingGameData_();
		if (nullptr == renderingGameData)
		{
			TA_ASSERT_DEV(false, "RenderingGameData가 없습니다.");
			return false;
		}

		if (ta::MeshType::Skeletal != renderingGameData->_meshType)
		{
			TA_ASSERT_DEV(false, "캐릭터액터인데 SkeletalMesh가 아닙니다.");
			return false;
		}

		//object->setStaticMesh()
		//character->setSkeletalMeshAndAnimInstance(renderingGameData->_meshPath.c_str(), renderingGameData->_animInstancePath.c_str());

	}

	TA_COMPILE_DEV(4 == static_cast<uint8>(ta::ActorType::Count), "여기도 추가해주세요");



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
		ATACharacter* character = Cast<ATACharacter>(actor->getUnrealActor_());
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

			TA_TEMP_DEV("임시 수정해야됨");
			{
				float factor = 0.1f;
				FVector directionVector = (destination - character->GetActorLocation());
				float distance = directionVector.Size();
				directionVector.Normalize();
				destination.X += directionVector.X * distance * factor;
				destination.Y += directionVector.Y * distance * factor;
			}

			destination.Z += character->getCharacterHalfHeight();

			aiController->MoveToLocation(destination, -1.0f, true, false);
			
#ifdef TA_MOVELOG
			TA_LOG_DEV("<MoveToLocation> => actorkey : %d, current position (%.1f, %.1f, %.1f)", actorKey.getKeyValue(), _destination._x, _destination._y, _destination._z);
#endif
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
		AActor* unrealActor = actor->getUnrealActor_();
		if (nullptr == unrealActor)
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

		unrealActor->SetActorLocation(position);
		unrealActor->SetActorRotation(rotation);
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

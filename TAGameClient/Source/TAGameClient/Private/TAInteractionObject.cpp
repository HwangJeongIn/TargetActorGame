// Fill out your copyright notice in the Description page of Project Settings.


#include "TAInteractionObject.h"

#include "TAGameInstance.h"
#include "TAPlayer.h"
#include "TACharacter.h"
//#include "TANonPlayer.h"
//#include "TAConvertFunctions.h"
#include "TAGameInstance.h"
#include "TAPlayerController.h"

//#include "TAAIController.h"
//#include "TAInventoryUserWidget.h"
//#include "Client/ClientApp.h"
//#include "Client/ClientActorManager.h"
//#include "Client/ClientActor.h"
//#include "Client/ClientMoveActorComponent.h"
//#include "Client/ClientCharacterActorComponent.h"
//#include "Common/ScopedLock.h"
//#include "Common/GetComponentAndSystem.h"
//#include "Common/GameData.h"



UTAInteractionObject::UTAInteractionObject()
{

}

bool UTAInteractionObject::execute(void) const noexcept
{
	return true;
}

void UTAInteractionObject::setInfo(ta::ActorKey myActorKey, ta::ActorKey targetActorKey) noexcept
{
	_myActorKey = myActorKey;
	_targetActorKey = targetActorKey;
}


UTAInteractionObjectLookAt::UTAInteractionObjectLookAt()
{
	_interactionObjectType = TAInteractionObjectType::LookAt;
}

bool UTAInteractionObjectLookAt::execute(void) const noexcept
{
	TA_LOG_DEV("UTAInteractionObjectLookAt : me(%d), target(%d)", _myActorKey.getKeyValue(), _targetActorKey.getKeyValue());
	TWeakObjectPtr<AActor> myCharacter = TAGetTAActor(_myActorKey);
	if (false == myCharacter.IsValid())
	{
		TA_ASSERT_DEV(false, "nullptr == myCharacter");
		return false;
	}

	TWeakObjectPtr<AActor> targetCharacter = TAGetTAActor(_targetActorKey);
	if (false == targetCharacter.IsValid())
	{
		TA_ASSERT_DEV(false, "nullptr == targetCharacter");
		return false;
	}

	FVector myPos = myCharacter->GetActorLocation();
	FVector targetPos = targetCharacter->GetActorLocation();

	//TA_LOG_DEV("UTAInteractionObjectLookAt : me(%d), target(%d)", _myActorKey.getKeyValue(), _targetActorKey.getKeyValue());
	targetCharacter->SetActorRotation(FRotationMatrix::MakeFromZ(myPos - targetPos).ToQuat());
	return true;
}



UTAInteractionObjectStopAndLookAt::UTAInteractionObjectStopAndLookAt()
{
	_interactionObjectType = TAInteractionObjectType::StopAndLookAt;
}

bool UTAInteractionObjectStopAndLookAt::execute(void) const noexcept
{
	TA_LOG_DEV("UTAInteractionObjectStopAndLookAt : me(%d), target(%d)", _myActorKey.getKeyValue(), _targetActorKey.getKeyValue());
	return true;
}


UTAInteractionObjectOpenDialog::UTAInteractionObjectOpenDialog()
{
	_interactionObjectType = TAInteractionObjectType::OpenDialog;
}

bool UTAInteractionObjectOpenDialog::execute(void) const noexcept
{
	TA_LOG_DEV("UTAInteractionObjectOpenDialog : me(%d), target(%d)", _myActorKey.getKeyValue(), _targetActorKey.getKeyValue());

	if (false == TAGetFirstPlayerController()->openDialog(_targetActorKey))
	{
		TA_ASSERT_DEV(false, "비정상");
		return false;
	}

	return true;
}
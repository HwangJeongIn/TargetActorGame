// Fill out your copyright notice in the Description page of Project Settings.


#include "TAChunkUserWidget.h"
#include "TAPlayerController.h"


UTAChunkUserWidget::UTAChunkUserWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	_boundaryType = TAUiBoundaryType::None;
}

bool UTAChunkUserWidget::initializeBase(ATACharacter* ownerActor, const TAUiBoundaryType& boundaryType) noexcept
{
	_boundaryType = boundaryType;

	if ((nullptr == _ownerActor.Get())
		|| (false == _ownerActor.IsValid()))
	{
		_ownerActor = ownerActor;
		return true;
	}

	TA_LOG_DEV("OwnerActor is already set");
	return false;
}

ATACharacter* UTAChunkUserWidget::getOwnerActor(void) noexcept
{
	if (nullptr == _ownerActor.Get())
	{
		TA_LOG_DEV("OwnerActor is nullptr");
		return nullptr;
	}
	
	if(false == _ownerActor.IsValid())
	{
		TA_LOG_DEV("OwnerActor is invalid ptr");
		return nullptr;
	}

	return _ownerActor.Get();
}

AController* UTAChunkUserWidget::getOwnerController(void) noexcept
{
	ATACharacter* character = getOwnerActor();
	if (nullptr == character)
	{
		return nullptr;
	}

	return character->GetController();
}

const TAUiBoundaryType UTAChunkUserWidget::getBoundaryType(void) noexcept
{
	return _boundaryType;
}

//const bool UTAChunkUserWidget::getVisibility(void) const noexcept
//{
//	return (ESlateVisibility::Visible == GetVisibility());
//}
//
//void UTAChunkUserWidget::setVisibility(const bool isVisible) noexcept
//{
//	if (isVisible == getVisibility())
//	{
//		return;
//	}
//
//	ESlateVisibility visibility = (true == isVisible) ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
//	SetVisibility(visibility);
//}

void UTAChunkUserWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	ATACharacter* target = getOwnerActor();
	ATAPlayerController* playerController = Cast<ATAPlayerController>(target->GetController());
	if (nullptr != playerController)
	{
		playerController->onHoveredUi(this);
	}
}

void UTAChunkUserWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	ATACharacter* target = getOwnerActor();
	ATAPlayerController* playerController = Cast<ATAPlayerController>(target->GetController());
	if (nullptr != playerController)
	{
		playerController->onUnhoveredUi(this);
	}
}
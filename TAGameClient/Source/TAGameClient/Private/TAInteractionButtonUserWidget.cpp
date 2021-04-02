// Fill out your copyright notice in the Description page of Project Settings.


#include "TAInteractionButtonUserWidget.h"
#include "TAInteractionObject.h"
#include "TAInteractionMenuUserWidget.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Components/Image.h"
#include "Common/CommonBase.h"
#include "Common/GameData.h"



UTAInteractionButtonUserWidget::UTAInteractionButtonUserWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UTAInteractionButtonUserWidget::doInteraction(void) const noexcept
{
	for (UTAInteractionObject* interactionObject : _interactionObjects)
	{
		if (false == interactionObject->execute())
		{
			TA_ASSERT_DEV(false, "비정상")
			return false;
		}
	}

	return true;
}

bool UTAInteractionButtonUserWidget::setInteractionObjectsByInteractionType(const ta::ActorKey& myActorKey, const ta::ActorKey& targetActorKey, const ta::InteractionType& interactionType) noexcept
{
	clearInteractionObjects();

	switch (interactionType)
	{
	case ta::InteractionType::Attack:
		{

		}
		break;
	case ta::InteractionType::Greet:
		{
			_interactionName->SetText(FText::FromString(TEXT("인사하기")));
			UTexture2D* texture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, TEXT("/Game/_UI/Textures/InteractionIcon.InteractionIcon")));
			_interactionImage->SetBrushFromTexture(texture);
		}
		break;
	case ta::InteractionType::Gather:
		{
			_interactionName->SetText(FText::FromString(TEXT("채집하기")));
			UTexture2D* texture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, TEXT("/Game/_UI/Textures/InteractionIcon.InteractionIcon")));
			_interactionImage->SetBrushFromTexture(texture);
		}
		break;
	case ta::InteractionType::Talk:
		{
			_interactionName->SetText(FText::FromString(TEXT("대화하기")));
			UTexture2D* texture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, TEXT("/Game/_UI/Textures/InteractionIcon.InteractionIcon")));
			_interactionImage->SetBrushFromTexture(texture);
			// 아직 Stop구현을 못해서 LookAt만
			//UTAInteractionObjectStopAndLookAt* stopAndLook = NewObject<UTAInteractionObjectStopAndLookAt>();
			{
				UTAInteractionObjectStopAndLookAt* interactionObject = NewObject<UTAInteractionObjectStopAndLookAt>();
				interactionObject->setInfo(myActorKey, targetActorKey);
				_interactionObjects.Add(interactionObject);
			}

			// dialog도 열어준다.
			{
				UTAInteractionObjectOpenDialog* interactionObject = NewObject<UTAInteractionObjectOpenDialog>();
				interactionObject->setInfo(myActorKey, targetActorKey);
				_interactionObjects.Add(interactionObject);
			}
		}
		break;
	default:
		{
			
		}
		break;
		TA_COMPILE_DEV(4 == static_cast<ta::uint8>(ta::InteractionType::Count), "여기도 확인해주세요");
	}
	return true;
}

void UTAInteractionButtonUserWidget::clearInteractionObjects(void) noexcept
{
	if (0 < _interactionObjects.Num())
	{
		_interactionObjects.Empty();
	}
}

void UTAInteractionButtonUserWidget::setInfo(UTAInteractionMenuUserWidget* ownerInteractionMenu) noexcept
{
	_ownerInteractionMenu = ownerInteractionMenu;
}

void UTAInteractionButtonUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	_interactionButton = Cast<UButton>(GetWidgetFromName(TEXT("interactionButton")));
	TA_ASSERT_DEV(nullptr != _interactionButton, "비정상");
	_interactionButton->OnPressed.AddDynamic(this, &UTAInteractionButtonUserWidget::onInteractionButtonPressed);

	_interactionImage = Cast<UImage>(GetWidgetFromName(TEXT("interactionImage")));
	TA_ASSERT_DEV(nullptr != _interactionImage, "비정상")

	_interactionName = Cast<UEditableText>(GetWidgetFromName(TEXT("interactionName")));
	TA_ASSERT_DEV(nullptr != _interactionName, "비정상")
}

void UTAInteractionButtonUserWidget::onInteractionButtonPressed() noexcept
{
	TA_LOG_DEV("onInteractionButtonPressed");
	if (false == doInteraction())
	{
		TA_ASSERT_DEV(false, "비정상");
	}
}

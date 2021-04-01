// Fill out your copyright notice in the Description page of Project Settings.


#include "TAInteractionMenuUserWidget.h"
#include "TAInteractionButtonUserWidget.h"
#include "Components/HorizontalBox.h"
#include "Blueprint/WidgetTree.h"
#include "TAPlayerController.h"



UTAInteractionMenuUserWidget::UTAInteractionMenuUserWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UTAInteractionButtonUserWidget> InteractionButton_C(TEXT("/Game/_UI/InteractionButton.InteractionButton_C"));
	TA_ASSERT_DEV(true == InteractionButton_C.Succeeded(), "비정상");
	if (true == InteractionButton_C.Succeeded())
	{
		_interactionButtonClass = InteractionButton_C.Class;
	}
}

void UTAInteractionMenuUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TA_LOG_DEV("UTAInteractionMenuUserWidget::NativeConstruct")

	_interactionButtonWrap = Cast<UHorizontalBox>(GetWidgetFromName(TEXT("interactionButtonWrap")));
	TA_ASSERT_DEV(nullptr != _interactionButtonWrap, "비정상")

	if (false == initializeInteractionButtons())
	{
		TA_ASSERT_DEV(false, "비정상");
	}
}

bool UTAInteractionMenuUserWidget::initializeInteractionButtons(void) noexcept
{
	if (nullptr == WidgetTree)
	{
		TA_ASSERT_DEV(false, "비정상");
		return false;
	}

	if (nullptr == _interactionButtonWrap)
	{
		TA_ASSERT_DEV(false, "비정상");
		return false;
	}

	// 3개를 만든다.
	const uint32 buttonCount = 3;
	UTAInteractionButtonUserWidget* currentButton = nullptr;
	FString buttonString;

	ATAPlayerController* playerController = Cast<ATAPlayerController>(getOwnerController());
	if (nullptr == playerController)
	{
		TA_ASSERT_DEV(false, "비정상");
		return false;
	}

	for (int32 index = 0; index < buttonCount; ++index)
	{
		buttonString = FString::Printf(TEXT("interactionButton_%d"), index);

		currentButton = WidgetTree->ConstructWidget<UTAInteractionButtonUserWidget>(_interactionButtonClass, *buttonString);
		//currentButton->Padding = FMargin(3.0f);
		currentButton->setInfo(this);
		_interactionButtonWrap->AddChild(currentButton);
	}

	return true;
}

TWeakObjectPtr<UTAInteractionButtonUserWidget> UTAInteractionMenuUserWidget::getInteractionButton(const uint8& index) noexcept
{
	return Cast<UTAInteractionButtonUserWidget>(_interactionButtonWrap->GetChildAt(index));
}

void UTAInteractionMenuUserWidget::setVisibleButtons(const int32& count) noexcept
{
	const int32 buttonCount = _interactionButtonWrap->GetChildrenCount();
	int32 visibleButtonCount = (buttonCount < count) ? buttonCount : count;

	// count만큼 켜주고
	{
		for (int32 index = 0; index < visibleButtonCount; ++index)
		{
			UTAInteractionButtonUserWidget* button = Cast<UTAInteractionButtonUserWidget>(_interactionButtonWrap->GetChildAt(index));
			button->SetVisibility(ESlateVisibility::Visible);
		}
	}

	// 나머지는 꺼준다.
	{
		for (int32 index = visibleButtonCount; index < buttonCount; ++index)
		{
			UTAInteractionButtonUserWidget* button = Cast<UTAInteractionButtonUserWidget>(_interactionButtonWrap->GetChildAt(index));
			button->SetVisibility(ESlateVisibility::Hidden);
		}
	}

}
// Fill out your copyright notice in the Description page of Project Settings.


#include "TAHUDUserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Common/CommonBase.h"



void UTAHUDUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	_HPBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("HPBar")));
	TA_ASSERT_DEV(nullptr != _HPBar, "비정상")

	_EPBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("EPBar")));
	TA_ASSERT_DEV(nullptr != _EPBar, "비정상")

	_MPBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("MPBar")));
	TA_ASSERT_DEV(nullptr != _MPBar, "비정상")
}


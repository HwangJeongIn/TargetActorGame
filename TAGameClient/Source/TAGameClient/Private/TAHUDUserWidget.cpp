// Fill out your copyright notice in the Description page of Project Settings.


#include "TAHUDUserWidget.h"
//#include "TAInventoryUserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/PanelWidget.h"
#include "Components/CanvasPanelSlot.h"
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

	//_inventoryTest = Cast<UTAInventoryUserWidget>(GetWidgetFromName(TEXT("inventoryTest")));
	//TA_ASSERT_DEV(nullptr != _inventoryTest, "비정상")
}

UCanvasPanelSlot* UTAHUDUserWidget::addChildWidgetToPanel(UWidget* target, FName panelName /*= FName()*/) noexcept
{
	// 어느 패널의 자식 객체로 들어간경우 SetXXXInViewport함수를 사용하면 안된다. 해당 패널의 슬롯을 가지고와서 패널에 맞게 캐스팅후 상대적인 좌표를 지정한다.
	//{
	//	FVector2D viewportSize(0, 0);
	//	GetWorld()->GetGameViewport()->GetViewportSize(viewportSize);
	//	TA_LOG_DEV("viewport 1 : %.1f, %.1f", viewportSize.X, viewportSize.Y);
	//
	//	UCanvasPanelSlot* canvasPanelSlot = Cast<UCanvasPanelSlot>(_inventoryTest->Slot);
	//	if (nullptr == canvasPanelSlot)
	//	{
	//		TA_LOG_DEV("nullptr == canvasPanelSlot");
	//	}
	//	else
	//	{
	//		canvasPanelSlot->SetAnchors(FAnchors(1.0f, 0.5f));
	//		canvasPanelSlot->SetAlignment(FVector2D(0.5f, 0.5f));
	//		canvasPanelSlot->SetPosition(FVector2D(-((viewportSize.X / 4.0f) / 2.0f), 0.0f)); // 화면의 1/4사이즈로 인벤토리 할 예정 피벗이 중앙이므로 2로 나눈값으로 위치를 잡는다.
	//		canvasPanelSlot->SetSize(FVector2D(viewportSize.X / 4.0f, viewportSize.Y / 2.0f));
	//	}
	//}

	UPanelWidget* parent = nullptr;
	if (true == panelName.IsNone())
	{
		parent = Cast<UPanelWidget>(GetRootWidget());
	}
	else
	{
		parent = Cast<UPanelWidget>(GetWidgetFromName(panelName));
		if (nullptr == parent)
		{
			TA_ASSERT_DEV(false, "패널이름이 없습니다. 일단 루트패널에 추가합니다.");
			parent = Cast<UPanelWidget>(GetRootWidget());
		}
	}

	//TA_ASSERT_DEV(false, "Root Widget PanelName : %s", *(GetRootWidget()->GetName()));
	

	if (nullptr == parent)
	{
		TA_ASSERT_DEV(false, "비정상 PanelName : %s", *(panelName.ToString()));
		return nullptr;
	}

	UCanvasPanelSlot* rv = Cast<UCanvasPanelSlot>(parent->AddChild(target));
	return rv;
}


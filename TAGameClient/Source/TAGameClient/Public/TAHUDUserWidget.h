// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TAHUDUserWidget.generated.h"


class UProgressBar;
class UCanvasPanelSlot;
//class UTextBlock;
//class UTAInventoryUserWidget;

/**
 * 
 */
UCLASS()
class TAGAMECLIENT_API UTAHUDUserWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:
	UCanvasPanelSlot* addChildWidgetToPanel(UWidget* target, FName panelName = FName()) noexcept;

protected:

	virtual void NativeConstruct() override final;


private:

	UPROPERTY()
	UProgressBar* _HPBar;
	
	UPROPERTY()
	UProgressBar* _EPBar;
	
	UPROPERTY()
	UProgressBar* _MPBar;
	
	//UPROPERTY()
	//UTAInventoryUserWidget* _inventoryTest;
	
};

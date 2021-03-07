// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TAChunkUserWidget.h"
#include "TAHudUserWidget.generated.h"


class UProgressBar;
class UCanvasPanelSlot;
//class UTextBlock;
//class UTAInventoryUserWidget;

/**
 * 
 */
UCLASS()
class TAGAMECLIENT_API UTAHudUserWidget : public UTAChunkUserWidget
{
	GENERATED_BODY()
	

public:
	UCanvasPanelSlot* addChildWidgetToPanel(UWidget* target, FName panelName = FName()) noexcept;

protected:

	virtual void NativeConstruct() override final;


private:


	UPROPERTY()
	UProgressBar* _hpBar;
	
	UPROPERTY()
	UProgressBar* _epBar;
	
	UPROPERTY()
	UProgressBar* _mpBar;
	
	//UPROPERTY()
	//UTAInventoryUserWidget* _inventoryTest;
	
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TAHUDUserWidget.generated.h"


class UProgressBar;
class UTextBlock;

/**
 * 
 */
UCLASS()
class TAGAMECLIENT_API UTAHUDUserWidget : public UUserWidget
{
	GENERATED_BODY()
	

protected:

	virtual void NativeConstruct() override final;


private:

	UPROPERTY()
	UProgressBar* _HPBar;
	
	UPROPERTY()
	UProgressBar* _EPBar;
	
	UPROPERTY()
	UProgressBar* _MPBar;
	
};

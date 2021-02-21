// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TAPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TAGAMECLIENT_API ATAPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ATAPlayerController();

	virtual void PostInitializeComponents(void) override final;
	virtual void OnPossess(APawn* aPawn) override final;
	
protected:
	virtual void BeginPlay() override final;
	virtual void SetupInputComponent() override final;
};

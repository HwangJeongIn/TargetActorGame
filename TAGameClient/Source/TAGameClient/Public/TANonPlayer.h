// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TACharacter.h"
#include "TANonPlayer.generated.h"

/**
 * 
 */
UCLASS()
class TAGAMECLIENT_API ATANonPlayer : public ATACharacter
{
	GENERATED_BODY()
	
public:
	ATANonPlayer();


	virtual void Tick(float DeltaTime) override final;

	virtual void BeginPlay() override final;
};

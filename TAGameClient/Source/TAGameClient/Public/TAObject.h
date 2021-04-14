// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TAActor.h"
#include "TAObject.generated.h"

UCLASS()
class TAGAMECLIENT_API ATAObject : public AActor, public TAActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATAObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

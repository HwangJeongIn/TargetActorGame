// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TANotSpawnedActor.generated.h"

class UStaticMeshComponent;

UCLASS()
class TAGAMECLIENT_API ATANotSpawnedActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATANotSpawnedActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	// 테스트 결과 3개중에 static mesh로 결정

	//UPROPERTY(VisibleAnyWhere)
	//USceneComponent* _sceneComponent;

	//UPROPERTY(VisibleAnyWhere)
	//UPrimitiveComponent* _primitiveComponent;

	UPROPERTY(VisibleAnyWhere)
	UStaticMeshComponent* _staticMesh;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TAExporter.generated.h"

UCLASS()
class TAGAMECLIENT_API ATAExporter : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATAExporter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override final;

public:
	UPROPERTY(EditAnyWhere, Category = ExportMenu)
	bool _exportNavMesh;

	UPROPERTY(EditAnyWhere, Category = ExportMenu)
	bool _exportPathPoint;

	UPROPERTY(EditAnyWhere, Category = ExportMenu)
	bool _exportSpawnData;

};

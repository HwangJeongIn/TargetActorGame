// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TAAssets.generated.h"

/**
 * 
 */
UCLASS(config=TAAssets)
class TAGAMECLIENTSETTING_API UTAAssets : public UObject
{
	GENERATED_BODY()
	
public:
	UTAAssets();

	UPROPERTY(config)
	TArray<FSoftObjectPath> _skeletalMeshAssets;

	UPROPERTY(config)
	TArray<FSoftObjectPath> _staticMeshAssets;

	UPROPERTY(config)
	TArray<FSoftObjectPath> _animInstanceAssets;
	
};

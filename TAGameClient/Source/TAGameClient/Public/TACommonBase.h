// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TACommonBase.generated.h"


UENUM(/*BlueprintType*/)
enum class UiBoundaryType : uint8
{
	None = 0
	, Hud
	, Inventory
};
// Fill out your copyright notice in the Description page of Project Settings.


#include "TANotSpawnedActor.h"
#include "Components/StaticMeshComponent.h"
#include "Common/CommonBase.h"


bool GetFolderName(const FString& folderPath, FString& folderName, uint8 depth/* = 1*/) noexcept
{
	TA_LOG_DEV("GetFolderName => folderPath : %s", *folderPath);
	TArray<FString> splitedStrings;
	const int32 num = folderPath.ParseIntoArray(splitedStrings, TEXT("/"), true);

	if (num != (depth + 1))
	{
		TA_LOG_DEV("depth에 맞지않음 num : %d, depth : %d", num, depth);
		return false;
	}

	folderName = splitedStrings[num - 1];
	return true;
}


// Sets default values
ATANotSpawnedActor::ATANotSpawnedActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	_sceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("USceneComponent"));
	TA_ASSERT_DEV((nullptr != _sceneComponent), "비정상");
	RootComponent = _sceneComponent;


	_staticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("UStaticMeshComponent"));
	TA_ASSERT_DEV((nullptr != _staticMesh), "비정상");

	//_staticMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	_staticMesh->SetCollisionProfileName("NoCollision");
	_staticMesh->bIsEditorOnly = 1;
	_staticMesh->SetGenerateOverlapEvents(false);
	_staticMesh->SetEnableGravity(false);
	_staticMesh->SetupAttachment(_sceneComponent);

}

// Called when the game starts or when spawned
void ATANotSpawnedActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATANotSpawnedActor::setActorLocationAsStaticMeshBottom(const float staticMeshHeight) noexcept
{
	TA_LOG_DEV("StaticMeshHeight : %.1f", staticMeshHeight);
	_staticMesh->SetWorldLocation(GetActorLocation() + FVector(0.0f, 0.0f, (0.0f == staticMeshHeight) ? 0.0f : staticMeshHeight / 2.0f));
}


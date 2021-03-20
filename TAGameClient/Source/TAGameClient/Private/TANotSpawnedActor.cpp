// Fill out your copyright notice in the Description page of Project Settings.


#include "TANotSpawnedActor.h"
#include "Components/StaticMeshComponent.h"


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
	FVector actorLocation = GetActorLocation();
	FVector staticMeshWorldLocation = actorLocation + FVector(0.0f, 0.0f, 10.0f);
	_staticMesh->SetWorldLocation(staticMeshWorldLocation);
}

void ATANotSpawnedActor::setActorLocationAsStaticMeshBottom(const float staticMeshHeight) noexcept
{
	TA_LOG_DEV("StaticMeshHeight : %.1f", staticMeshHeight);
	
	// 상대 위치 써야하는 이유
	// 1. 절대 위치로 계산해도 어차피 내부에서 액터 좌표계로 매핑하기 때문에 상대위치로 하는게 좋다
	// 2. 절대 위치로 계산할때 해당 액터의 Up벡터를 가지고 와서 계산해야한다.
	//{
	//	FVector actorLocation = GetActorLocation();
	//	TA_LOG_DEV("GetActorLocation : %.1f, %.1f, %.1f", actorLocation.X, actorLocation.Y, actorLocation.Z);
	//
	//	FVector staticMeshWorldLocation = actorLocation + GetActorUpVector() * ((0.0f == staticMeshHeight) ? 0.0f : staticMeshHeight / 2.0f);
	//	TA_LOG_DEV("StaticMeshWorldLocation : %.1f, %.1f, %.1f", staticMeshWorldLocation.X, staticMeshWorldLocation.Y, staticMeshWorldLocation.Z);
	//	_staticMesh->SetWorldLocation(staticMeshWorldLocation);
	//}

	_staticMesh->SetRelativeLocation(FVector(0.0f, 0.0f, (0.0f == staticMeshHeight) ? 0.0f : staticMeshHeight / 2.0f));
}


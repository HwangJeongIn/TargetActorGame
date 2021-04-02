// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Common/CommonBase.h"
#include "TANotSpawnedActor.generated.h"

// 에디팅시 켜고 작업
//#define FOR_EDITING

class UStaticMeshComponent;
class USceneComponent;
class ULevel;


template <typename T>
int32 TAGetTargetLevelActorsInFolder(ULevel* level, const FString& targetFolderName, TArray<T*>& output) noexcept
{
#ifndef FOR_EDITING
	TA_ASSERT_DEV(false, "에디터모드를 켜세요");
	return 0;
#else
	T* current = nullptr;
	FString currentFolderName;
	FString currentFolderPath;
	for (AActor* actor : level->Actors)
	{
		current = Cast<T>(actor);
		if (nullptr == current)
		{
			continue;
		}

		currentFolderPath = current->GetFolderPath().ToString();
		if (false == TAGetFolderName(currentFolderPath, currentFolderName))
		{
			continue;
		}

		TA_LOG_DEV("=> name : %s / folder path : %s", *current->GetName(), *currentFolderPath);
		if (targetFolderName != currentFolderName)
		{
			TA_LOG_DEV("actor is in different folder name");
			continue;
		}

		output.Add(current);
	}

	return output.Num();
#endif
}

template <typename T>
void TAGetTargetLevelActorsByFolder(ULevel* level, TMap<FString, TArray<T*>>& output) noexcept
{
#ifndef FOR_EDITING
	TA_ASSERT_DEV(false, "에디터모드를 켜세요");
	return;
#else
	// FString은 operator==지원 / GetTypeHash지원하기 때문에 TMap에서 키타입으로 그대로 사용가능
	// 2번조회하는 Contain -> []보다  Find으로 찾자

	T* current = nullptr;
	FString currentFolderName;
	FString currentFolderPath;
	TArray<T*>* currentFolderActors = nullptr;
	for (AActor* actor : level->Actors)
	{
		current = Cast<T>(actor);
		if (nullptr == current)
		{
			continue;
		}

		currentFolderPath = current->GetFolderPath().ToString();
		if (false == TAGetFolderName(currentFolderPath, currentFolderName))
		{
			continue;
		}

		TA_LOG_DEV("=> name : %s / folder path : %s", *current->GetName(), *currentFolderPath);

		currentFolderActors = output.Find(currentFolderName);
		if (nullptr == currentFolderActors)
		{
			output.Add(currentFolderName, TArray<T*>{ current });
		}
		else
		{
			currentFolderActors->Add(current);
		}
	}
#endif
}

extern bool TAGetFolderName(const FString& folderPath, FString& folderName, uint8 depth = 0) noexcept;


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

	void setActorLocationAsStaticMeshBottom(const float staticMeshHeight) noexcept;

protected:

	// 테스트 결과 2개중에 static mesh로 결정
	//UPROPERTY(VisibleAnyWhere)
	//UPrimitiveComponent* _primitiveComponent;

	UPROPERTY(VisibleAnyWhere)
	UStaticMeshComponent* _staticMesh;

private:
	// 액터자체에 transform정보를 넣어주기 위해서 root에 SceneComponent를 배치했다.
	// static mesh의 경우 위치 크기가 유동적으로 바뀔 수 있기 때문에 하위 컴포넌트로 추가 했다.
	// 특히 static mesh의 피봇을 그 mesh의 바닥으로 설정할 수 있고, 
	// 회전시킬때 Actor자체를 회전(Root = SceneComponent 의 transform와 동기화됨)를 회전시키면 설정된 피봇을 중심으로 회전시킬 수 있어서 편리하다.
	UPROPERTY(VisibleAnyWhere)
	USceneComponent* _sceneComponent;
};

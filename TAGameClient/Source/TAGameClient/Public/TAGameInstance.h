// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Engine/StreamableManager.h"
#include "Containers/Queue.h"
#include <vector>
#include "Common/Lockable.h"
#include "Common/KeyDefinition.h"
#include "TAGameInstance.generated.h"


class UTAGameInstance;
class UWorld;
class ATAPlayer;
class ATAPlayerController;
class ULevel;
class TAGameEvent;
class ATANonPlayer;
class ATACharacter;

namespace ta
{
	class ActorKey;
}


extern UTAGameInstance* TAGetGameInstance(void) noexcept;
extern UWorld* TAGetGameWorld(void) noexcept;

extern ULevel* TAGetStreamingLevelByName(const FString& levelName) noexcept;
extern int32 TAGetStreamingLevelByTag(const FString& tag, TArray<ULevel*>& levelOutput, TArray<FString>* nameWithoutTagOutput = nullptr) noexcept;
extern FString TAGetLevelNameWithoutTag(const ULevel* level) noexcept;
extern FString TATrimLevelTag(const FString& levelNameWithTag) noexcept;
extern void TAPrintAllStreamingLevelName(void) noexcept; // test

extern ATAPlayerController* TAGetFirstPlayerController(void) noexcept;
extern ATAPlayer* TAGetFirstPlayer(void) noexcept;

extern ATACharacter* TASpawnTAActor(const ta::ActorKey& actorKey, const FVector& position, const FRotator& rotation) noexcept;
extern bool TADestroyTAActor(const ta::ActorKey& actorKey) noexcept;
extern TWeakObjectPtr<ATACharacter> TAGetTAActor(const ta::ActorKey& actorKey) noexcept;

extern bool TAExportRecastNavMesh(void) noexcept;

extern bool TAExportPathPoint(void) noexcept;
extern bool TAExportLevelPathPoint(ULevel* level) noexcept;

extern bool TAExportSpawnData(void) noexcept;
extern bool TAExportLevelSpawnData(ULevel* level) noexcept;


/**
 * 
 */
UCLASS()
class TAGAMECLIENT_API UTAGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UTAGameInstance();
	// 가비지 컬렉션 사용하기 때문에 Shutdown함수 사용하자
	//virtual ~UTAGameInstance();

	virtual void StartGameInstance() override final;
	virtual void OnStart() override final;
	virtual void Shutdown() override final;

	bool exportNavMesh(void) noexcept;
	void processTimerTick(void) noexcept;
	void processGameEventQueue(const float deltaTime) noexcept;
	void processFindInteractionActor(const float deltaTime) noexcept;
	void processInteractionMenu(ATAPlayerController* playerController) noexcept;

	bool registerGameEvent(TAGameEvent* gameEvent) noexcept;
	void clearGameEvents(void) noexcept;

	ATACharacter* spawnTAActor(const ta::ActorKey& actorKey, const FVector& position, const FRotator& rotation) noexcept;
	bool destroyTAActor(const ta::ActorKey& actorKey) noexcept;
	TWeakObjectPtr<ATACharacter> getTAActor(const ta::ActorKey& actorKey) noexcept;

	FStreamableManager& getStreamableManager(void) noexcept;

	FSoftObjectPath getSkeletalMeshAssetPath(const FString& key) noexcept;
	FSoftObjectPath getStaticMeshAssetPath(const FString& key) noexcept;
	FSoftObjectPath getAnimInstanceAssetPath(const FString& key) noexcept;

	const ta::ActorKey& getCurrentInteractionActorKey(void) noexcept;

private:
	// UE4 자체 TQueue로 c++ 클래스 TAGameEvent 객체들을 담았다가 일단 문제 발생할것 같아서 바꾸었다.
	// UPROPERTY로 가비지 수집되는 것을 막을 순 있지만, 그러려면 객체가 UCLASS나 USTRUCT이여야 하는데 
	// UCLASS의 UObject객체로 하자니 여러 스레드에서 생성하게 되어서 문제 생길 수 있다. // UObject는 따로 관리받는 객체
	// USTRUCT의 경우는 시도해봤지만 빌드는 UPROPERTY없을 때 잘되었지만, UPROPERTY를 붙이는 순간 불가능하다고 컴파일 에러난다.
	// 추후 좋은 방법이 있으면 바꿀 예정이다. (TQueue가 Thread Safe하기 때문에 락이 따로 필요없어서 빠르긴하다.. 방법고민중)
	std::vector<TAGameEvent*> _gameEventQueue;
	ta::LockableObject _gameEventQueueLock;


	UPROPERTY()
	TMap<uint32, ATANonPlayer*> _spawnedTAActors;

	ta::ActorKey _currentInteractionActorKey;
	
	float _findInteractionActorInterval;
	float _currentFindInteractionActorTime;


	// 비동기 로드를 위한 스트림 매니저
	//UPROPERTY()
	FStreamableManager _streamableManager;

	// asset table key finder // 로드된 에셋을 선형적으로 돌면서 계속 찾을 순 없으니 해당 에셋의 인덱스를 찾는 테이블을 마련해둔다.
	UPROPERTY()
	TMap<FString, uint32> _skeletalMeshAssetKeyFinder;

	UPROPERTY()
	TMap<FString, uint32> _staticMeshAssetKeyFinder;

	UPROPERTY()
	TMap<FString, uint32> _animInstanceAssetKeyFinder;

	UPROPERTY(VisibleAnyWhere, Category = GameControl)
	bool _navMeshExported;
};
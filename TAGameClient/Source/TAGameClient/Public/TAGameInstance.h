// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Containers/Queue.h"
#include "TAGameEvent.h"
#include "TAGameInstance.generated.h"


class UTAGameInstance;
class UWorld;
class ATAPlayer;
class ATAPlayerController;


extern UTAGameInstance* GetTAGameInstance(void) noexcept;
extern UWorld* GetTAGameWorld(void) noexcept;
extern ATAPlayerController* GetFirstTAPlayerController(void) noexcept;
extern ATAPlayer* GetFirstTAPlayer(void) noexcept;

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
	void processGameEventQueue(void) noexcept;
	bool registerGameEvent(TAGameEvent* gameEvent) noexcept;
	void clearGameEvents(void) noexcept;

private:
	TQueue<TAGameEvent*> _gameEventQueue;

	UPROPERTY(VisibleAnyWhere, Category = GameControl)
	bool _navMeshExported;
};
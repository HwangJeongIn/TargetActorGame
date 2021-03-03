// Fill out your copyright notice in the Description page of Project Settings.


#include "TAGameInstance.h"
#include "Client/ClientApp.h"
//#include "Engine.h" 
//#include "Engine/Engine.h" 
#include "Engine/GameEngine.h" 
#include "TAGameMode.h"
#include "TACharacter.h"
#include "TAPlayer.h"
#include "TAPlayerController.h"
#include "TimerManager.h"
#include "NavigationSystem.h"
#include "NavMesh/RecastNavMesh.h"
#include "Runtime/Engine/Public/EngineUtils.h"
#include "Common/GameDataManager.h"
#include "Common/CommonSpawnDataManager.h"



UTAGameInstance* GetTAGameInstance(void) noexcept
{
	if (nullptr != GEngine
		&& nullptr != GEngine->GameViewport)
	{
		//UTAGameInstance* gameInstance = Cast<UTAGameInstance>(GEngine->GameViewport->GetWorld()->GetGameInstance());
		UTAGameInstance* gameInstance = Cast<UTAGameInstance>(GEngine->GameViewport->GetGameInstance());

		return gameInstance;
	}

	//UGameEngine* gameEngine = Cast<UGameEngine>(GEngine); // nullptr로 나온다..
	//if (nullptr != gameEngine)
	//{
	//	UTAGameInstance* gameInstance = Cast<UTAGameInstance>(gameEngine->GameInstance);
	//	return gameInstance;
	//}

	return nullptr;
}

UWorld* GetTAGameWorld(void) noexcept
{
	if (nullptr != GEngine
		&& nullptr != GEngine->GameViewport)
	{
		return GEngine->GameViewport->GetWorld();
	}

	return nullptr;
}

ATAPlayerController* GetFirstTAPlayerController(void) noexcept
{
	UWorld* world = GetTAGameWorld();
	if (nullptr == world)
	{
		TA_ASSERT_DEV(false, "비정상입니다.");
		return nullptr;
	}

	//world->GetFirstLocalPlayerFromController
	ATAPlayerController* playerController = Cast<ATAPlayerController>(world->GetFirstPlayerController());
	if (nullptr == playerController)
	{
		TA_ASSERT_DEV(false, "비정상입니다.");
		return nullptr;
	}

	return playerController;
}

ATAPlayer* GetFirstTAPlayer(void) noexcept
{
	ATAPlayerController* playerController = GetFirstTAPlayerController();
	if (nullptr == playerController)
	{
		TA_ASSERT_DEV(false, "비정상입니다.");
		return nullptr;
	}
	
	ATAPlayer* player = Cast<ATAPlayer>(playerController->GetPawn());
	if (nullptr == player)
	{
		TA_ASSERT_DEV(false, "비정상입니다.");
		return nullptr;
	}

	return player;
}


UTAGameInstance::UTAGameInstance()
{
	_navMeshExported = false;
	{
		//TA_LOG_DEV("%s", *FPaths::GetProjectFilePath());
		//TA_LOG_DEV("test log");
		//TA_LOG_DEV("%s", fs::current_path().c_str());

		ta::GameDataXmlPath = *FPaths::GetProjectFilePath();
		ta::GameDataXmlPath /= "../GameData/Xml";
		TA_LOG_DEV("%s", ta::GameDataXmlPath.c_str());


		ta::SpawnDataPath = *FPaths::GetProjectFilePath();
		ta::SpawnDataPath /= "../SpawnData";
		TA_LOG_DEV("%s", ta::SpawnDataPath.c_str());
	}
}


//UTAGameInstance::~UTAGameInstance()
//{
//	//static_cast<ta::ClientApp*>(ta::g_app)->waitAndClose();
//	delete ta::g_app;
//}

void UTAGameInstance::StartGameInstance()
{
	Super::StartGameInstance();
	TA_LOG_DEV("StartGameInstance");

	//TA_LOG_DEV("%s", *FPaths::GetProjectFilePath());
	//TA_LOG_DEV("test log");
}

void UTAGameInstance::OnStart()
{
	Super::OnStart();
	TA_LOG_DEV("OnStart");

	//if (false == exportNavMesh())
	//{
	//	TA_ASSERT_DEV(false, "비정상입니다.");
	//}

	GetTimerManager().SetTimerForNextTick(this, &UTAGameInstance::processGameEventQueue);
	
	static_cast<ta::ClientApp*>(ta::g_app)->run();
}

void UTAGameInstance::Shutdown()
{
	Super::Shutdown();

	clearGameEvents();


	//delete ta::g_app;
}

bool UTAGameInstance::exportNavMesh(void) noexcept
{
	if (true == _navMeshExported)
	{
		return true;
	}

	UWorld* world = GetWorld();
	UNavigationSystemV1* navSystem = UNavigationSystemV1::GetNavigationSystem(world);
	if (nullptr == navSystem)
	{
		TA_ASSERT_DEV(false, "네비시스템이 없습니다.");
		return false;
	}
	
	ANavigationData* navData = nullptr;
	for (TActorIterator<ATACharacter> entity(world); entity; ++entity) // for (const auto& entity : TActorRange<AStaticMeshActor>(world))
	{
		AController* controller = entity->GetController();
		const FNavAgentProperties& agentProps = controller->GetNavAgentPropertiesRef();
		navData = navSystem->GetNavDataForProps(agentProps, FVector::ZeroVector);
		TA_LOG_DEV("시작");
		break;
	}

	//ANavigationData* navData = navSystem->GetDefaultNavDataInstance(FNavigationSystem::DontCreate);
	//if (nullptr == navData)
	//{
	//	navData = navSystem->GetDefaultNavDataInstance();
	//}

	if (nullptr == navData)
	{
		TA_ASSERT_DEV(false, "네비데이터가 없습니다..");
		return false;
	}


	ARecastNavMesh* navMesh = Cast<ARecastNavMesh>(navData);
	if (nullptr == navMesh)
	{
		TA_ASSERT_DEV(false, "네비메시가 없습니다.");
		return false;
	}
	
	/*
	UNavigationSystemBase* navSystem = GetWorld()->GetNavigationSystem();
	if (nullptr == navSystem)
	{
		TA_ASSERT_DEV(false, "네비시스템이 없습니다.");
		return false;
	}

	//GetWorld()->GetNavigationSystem()->GetMainNavData()->GetGenerator()->ExportNavigationData(FString("C:\\data.txt"));

	ANavigationData* NavData = Cast<ANavigationData>(navSystem->GetMainNavData());
	ARecastNavMesh* NavMesh = Cast<ARecastNavMesh>(NavData);
	if (nullptr == NavMesh)
	{
		TA_ASSERT_DEV(false, "네이메시가 없습니다.");
		return false;
	}*/

	TArray<FNavPoly> NavPolyIDs;
	for (int i = 0; i < navMesh->GetNavMeshTilesCount(); i++)
	{
		const FBox TileBounds = navMesh->GetNavMeshTileBounds(i);

		if (!TileBounds.IsValid)
			continue;

		navMesh->GetPolysInTile(i, NavPolyIDs);
	}

	TArray<FVector> Verts;
	for (int i = 0; i < NavPolyIDs.Num(); i++)
	{
		Verts.Reset();
		if (navMesh->GetPolyVerts(NavPolyIDs[i].Ref, Verts))
		{
			TA_LOG_DEV("Poly start, num verts: %d", Verts.Num());
			for (int j = 0; j < Verts.Num(); ++j)
			{
				FVector Vert = Verts[j];
				TA_LOG_DEV("Vertex position: %f %f %f", Vert.X, Vert.Y, Vert.Z);
			}
			TA_LOG_DEV("Poly end");
		}
	}

	TA_LOG_DEV("export end");
	navMesh->GetGenerator()->ExportNavigationData(FString("E:/GitLocal/TargetActor/nav.txt"));

	_navMeshExported = true;
	return true;
}

void UTAGameInstance::processGameEventQueue(void) noexcept
{
	//TA_LOG_DEV("processGameEvent");
	GetTimerManager().SetTimerForNextTick(this, &UTAGameInstance::processGameEventQueue);

	if (true == _gameEventQueue.IsEmpty())
	{
		return;
	}

	TAGameEventProcessParameter parameter;
	parameter._gameInstance = this;
	parameter._world = GetWorld();

	while (false == _gameEventQueue.IsEmpty())
	{
		TAGameEvent* output = nullptr;
		if (true == _gameEventQueue.Dequeue(output))
		{
			TA_LOG_DEV("GameEventType : %d, ActorKey : %d", output->getGameEventType(), output->_actorKey.getKeyValue());
			if (false == output->processEvent(parameter))
			{
				TA_ASSERT_DEV(false, "비정상입니다");
			}
			delete output;
		}
	}
}

bool UTAGameInstance::registerGameEvent(TAGameEvent* gameEvent) noexcept
{
	return _gameEventQueue.Enqueue(gameEvent);
}

void UTAGameInstance::clearGameEvents(void) noexcept
{
	while (false == _gameEventQueue.IsEmpty())
	{
		TAGameEvent* output = nullptr;
		if (true == _gameEventQueue.Dequeue(output))
		{
			delete output;
		}
	}
}
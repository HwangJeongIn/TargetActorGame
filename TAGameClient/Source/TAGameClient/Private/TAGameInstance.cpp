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
#include "TAPathPoint.h"
#include "TASpawnData.h"
#include "TimerManager.h"
#include "NavigationSystem.h"
#include "NavMesh/RecastNavMesh.h"
#include "Runtime/Engine/Public/EngineUtils.h"
#include "NavigationSystem.h"
#include "LevelEditor.h"
#include "Editor.h"
#include "Engine/LevelStreaming.h"

#include "Common/GameDataManager.h"
#include "Common/CommonSpawnDataManager.h"
#include "Common/CommonMoveActorSystem.h"
#include "Common/Serializer.h"
#include "Common/FileLoader.h"
#include "Common/GetComponentAndSystem.h"
#include "Common/StringUtility.h"


UTAGameInstance* TAGetGameInstance(void) noexcept
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

UWorld* TAGetGameWorld(void) noexcept
{
	if(nullptr == GEngine
	   || nullptr == GEngine->GameViewport)
	{
		TA_LOG_DEV("에디터에서 월드를 찾습니다.");
		if (nullptr == GEditor)
		{
			TA_ASSERT_DEV(false, "월드가 존재하지 않습니다.");
			return nullptr;
		}

		if (nullptr == GEditor->PlayWorld)
		{
			TA_LOG_DEV("GEditor->PlayWorld가 nullptr입니다.");
			UWorld* world = GEditor->GetEditorWorldContext().World();
			TA_ASSERT_DEV(nullptr != world, "비정상입니다.");
			return world;
		}

		return GEditor->PlayWorld;
	}

	UWorld* world = GEngine->GameViewport->GetWorld();
	TA_ASSERT_DEV(nullptr != world, "비정상입니다.");
	return world;
}

ULevel* TAGetStreamingLevelByName(const FString& levelName) noexcept
{
	const TArray<ULevelStreaming*>& streamingLevels = TAGetGameWorld()->GetStreamingLevels();

	for (const ULevelStreaming* streamingLevel: streamingLevels)
	{
		if (nullptr == streamingLevel)
		{
			continue;
		}

		ULevel* level = streamingLevel->GetLoadedLevel();

		if (nullptr == level
			|| false == level->bIsVisible)
		{
			continue;
		}

		if (levelName == (level->GetOuter()->GetName()))
		{
			return level;
		}
	}

	return nullptr;
}

int32 TAGetStreamingLevelByTag(const FString& tag, TArray<ULevel*>& levelOutput, TArray<FString>* nameWithoutTagOutput/* = nullptr*/) noexcept
{
	levelOutput.Empty();
	if (nullptr != nameWithoutTagOutput)
	{
		nameWithoutTagOutput->Empty();
	}

	const TArray<ULevelStreaming*>& streamingLevels = TAGetGameWorld()->GetStreamingLevels();

	for (const ULevelStreaming* streamingLevel: streamingLevels)
	{
		if (nullptr == streamingLevel)
		{
			continue;
		}

		ULevel* level = streamingLevel->GetLoadedLevel();

		if (nullptr == level
			|| false == level->bIsVisible)
		{
			continue;
		}

		TArray<FString> splitedString;
		FString levelName = (level->GetOuter()->GetName());
		levelName.ParseIntoArray(splitedString, TEXT("_"));

		if (tag == splitedString.Last())
		{
			TA_LOG_DEV("tag : %s, level name : %s", *tag, *levelName);
			levelOutput.Add(level);
			if (nullptr != nameWithoutTagOutput)
			{
				nameWithoutTagOutput->Add(splitedString[0]);
			}
		}
	}

	return levelOutput.Num();
}

extern FString TAGetLevelNameWithoutTag(const ULevel* level) noexcept
{
	if (nullptr == level)
	{
		return FString();
	}

	return TATrimLevelTag(level->GetOuter()->GetName());
}

extern FString TATrimLevelTag(const FString& levelNameWithTag) noexcept
{
	TArray<FString> splitedString;
	levelNameWithTag.ParseIntoArray(splitedString, TEXT("_"));
	return splitedString[0];
}

void TAPrintAllStreamingLevelName(void) noexcept
{
	const TArray<ULevelStreaming*>& streamingLevels = TAGetGameWorld()->GetStreamingLevels();

	TA_LOG_DEV("StreamedLevels : %d", streamingLevels.Num());
	for (const ULevelStreaming* streamingLevel : streamingLevels)
	{
		if (nullptr == streamingLevel)
		{
			continue;
		}

		ULevel* level = streamingLevel->GetLoadedLevel();

		//Is This Level Valid and Visible?
		if (nullptr == level)
		{
			continue;
		}

		if (false == level->bIsVisible)
		{
			TA_LOG_DEV("Level Name InVisible : %s", *(level->GetOuter()->GetName()));
			continue;
		}

		TA_LOG_DEV("Level Name Visible : %s", *(level->GetOuter()->GetName()));
	}
}

ATAPlayerController* TAGetFirstPlayerController(void) noexcept
{
	UWorld* world = TAGetGameWorld();
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

ATAPlayer* TAGetFirstPlayer(void) noexcept
{
	ATAPlayerController* playerController = TAGetFirstPlayerController();
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

bool TAExportRecastNavMesh(void) noexcept
{
	UNavigationSystemV1* navSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(TAGetGameWorld());
	if (nullptr == navSys)
	{
		TA_ASSERT_DEV(false, "월드가 존재하지 않습니다.");
		return false;
	}


	// 결국 UNavigationSystemV1.SupportedAgent가 1이기 때문에 무조건 굳이 AgentProperty로 맵에서 찾을 필요가 없다.
	// 지금은 무조건 UNavigationSystemV1.MainNavData가 나온다.
//#define SupportOtherAgent
#ifdef SupportOtherAgent

	ATAPlayerController* playerController = TAGetFirstPlayerController();
	if (nullptr == playerController)
	{
		TA_ASSERT_DEV(false, "비정상입니다.");
		return false;
	}

	//FNavAgentProperties nagAgentProperties;
	//nagAgentProperties.bCanCrouch = 0;
	//nagAgentProperties.bCanJump = 1;
	//nagAgentProperties.bCanWalk = 1;
	//nagAgentProperties.bCanSwim = 1;
	//nagAgentProperties.bCanFly = 0;
	//
	//nagAgentProperties.AgentRadius = 34.0f;
	//nagAgentProperties.AgentHeight = 142.0f;
	//nagAgentProperties.AgentStepHeight = -1.0f;
	//nagAgentProperties.NavWalkingSearchHeightScale = 0.5f;

	ANavigationData* navData = (navSys == nullptr) ? nullptr : navSys->GetNavDataForProps(playerController->GetNavAgentPropertiesRef()
						
#endif

	ARecastNavMesh* recastNavMesh = Cast<ARecastNavMesh>(navSys->MainNavData);
	if (nullptr == recastNavMesh)
	{
		TA_ASSERT_DEV(false, "비정상입니다.");
		return false;
	}

	dtNavMesh* detourNavMesh = recastNavMesh->GetRecastMesh();
	if (nullptr == detourNavMesh)
	{
		TA_ASSERT_DEV(false, "비정상입니다.");
		return false;
	}

	ta::Serializer slW;
	slW.setModeFlag(ta::SerializerMode::Write | ta::SerializerMode::WriteLog);
	if (false == ta::CommonMoveActorSystem::serializeNavigationMesh(slW, detourNavMesh))
	{
		TA_ASSERT_DEV(false, "비정상입니다.");
		return false;
	}

	fs::path finalPath = ta::NavigationMeshFilePath / "RecastNavigationMesh.rnm";
	if (false == slW.exportBinaryToFile(finalPath))
	{
		TA_ASSERT_DEV(false, "비정상입니다.");
		return false;
	}

#ifdef CAN_CREATE_LOG_FILE
	if (false == slW.exportLogData(finalPath += "Write"))
	{
		TA_ASSERT_DEV(false, "비정상");
		return false;
	}
#endif 

	return true;
}

bool TAExportPathPoint(void) noexcept
{
	TArray<ULevel*> pathPointLevels;
	const int32 levelCount = TAGetStreamingLevelByTag("pathpoint", pathPointLevels);

	if (0 == levelCount)
	{
		TA_LOG_DEV("path point level이 없습니다.");
		return true;
	}
	
	for (int32 levelIndex = 0; levelIndex < levelCount; ++levelIndex)
	{
		if (false == TAExportLevelPathPoint(pathPointLevels[levelIndex]))
		{
			TA_LOG_DEV("비정상입니다.");
			return false;
		}
	}


	return true;
}

bool TAExportLevelPathPoint(ULevel* level) noexcept
{
	TMap<FString, TArray<ATAPathPoint*>> allPathPointLevelFolders;
	TAGetTargetLevelActorsByFolder(level, allPathPointLevelFolders);

	std::string currentFolderName;
	for (TPair<FString, TArray<ATAPathPoint*>>& currentPathPointFolder : allPathPointLevelFolders)
	{
		// 사실상 wchar -> char(ASCII)
		currentFolderName = TCHAR_TO_ANSI(*currentPathPointFolder.Key);
		TArray<ATAPathPoint*>& currentPathPointGroup = currentPathPointFolder.Value;

		const int32 count = currentPathPointGroup.Num();
		ta::XmlNode tempRoot("Root");

		ta::XmlNode* child = nullptr;
		ta::XmlNode* childPositionNode = nullptr;
		ta::tstring positionValue;
		for (int32 index = 0; index < count; ++index)
		{
			child = new ta::XmlNode(currentFolderName.c_str());
			childPositionNode = new ta::XmlNode("Position");

			tempRoot.addChildElement(child);
			child->addChildElement(childPositionNode);

			FVector pathPointLocation = currentPathPointGroup[index]->GetActorLocation();
			
			childPositionNode->addAttribute("X", ta::ToStringCast<float>(pathPointLocation.X));
			childPositionNode->addAttribute("Y", ta::ToStringCast<float>(pathPointLocation.Y));
			childPositionNode->addAttribute("Z", ta::ToStringCast<float>(pathPointLocation.Z));
			
			//FormatString(positionValue, "(%.1f,%.1f,%.1f)", pathPointLocation.X, pathPointLocation.Y, pathPointLocation.Z);
			//child->addAttribute("Position", ta::ToString(positionValue));

		}

		if (false == ta::FileLoader::saveXml((ta::PathPointFilePath / currentFolderName) += ".xml", &tempRoot))
		{
			TA_ASSERT_DEV(false, "save xml failed : %s", currentFolderName.c_str());
			return false;
		}
	}

	return true;
}

bool TAExportSpawnData(void) noexcept
{
	TArray<ULevel*> spawnDataLevels;
	TArray<FString> levelNameWitoutTag;
	const int32 levelCount = TAGetStreamingLevelByTag("spawndata", spawnDataLevels, &levelNameWitoutTag);

	if (0 == levelCount)
	{
		TA_LOG_DEV("spawn data level이 없습니다.");
		return true;
	}


	for (int32 levelIndex = 0; levelIndex < levelCount; ++levelIndex)
	{
		if (false == TAExportLevelSpawnData(spawnDataLevels[levelIndex]))
		{
			TA_LOG_DEV("비정상입니다.");
			return false;
		}
	}

	return true;
}

extern bool TAExportLevelSpawnData(ULevel* level) noexcept
{
	TMap<FString, TArray<ATASpawnData*>> allSpawnDataLevelFolders;
	TAGetTargetLevelActorsByFolder(level, allSpawnDataLevelFolders);

	std::string currentLevelName = TCHAR_TO_ANSI(*TAGetLevelNameWithoutTag(level));
	std::string currentFolderName;

	for (TPair<FString, TArray<ATASpawnData*>>& currentSpawnLevelFolder : allSpawnDataLevelFolders)
	{
		TArray<ATASpawnData*>& spawnData = currentSpawnLevelFolder.Value;

		const int32 spawnDataCount = spawnData.Num();
		if (0 == spawnDataCount)
		{
			continue;
		}

		currentFolderName = TCHAR_TO_ANSI(*currentSpawnLevelFolder.Key);

		ta::XmlNode tempRoot("Root");
		ta::XmlNode* child = nullptr;
		ta::XmlNode* childPositionNode = nullptr;
		ta::XmlNode* childRotationNode = nullptr;
		ta::XmlNode* childGroupGameDataNode = nullptr;

		for (int32 spawnDataIndex = 0; spawnDataIndex < spawnDataCount; ++spawnDataIndex)
		{
			child = new ta::XmlNode("SpawnData");
			childPositionNode = new ta::XmlNode("Position");
			childRotationNode = new ta::XmlNode("Rotation");
			childGroupGameDataNode = new ta::XmlNode("GroupGameData");

			tempRoot.addChildElement(child);
			child->addChildElement(childGroupGameDataNode);
			child->addChildElement(childPositionNode);
			child->addChildElement(childRotationNode);

			{
				const ta::GroupGameDataKey groupGameDataKey(static_cast<ta::GroupGameDataKeyType>(spawnData[spawnDataIndex]->getGroupGameDataKey()));
				if (false == groupGameDataKey.isValid())
				{
					TA_ASSERT_DEV(false, "비정상입니다.");
					return false;
				}

				childGroupGameDataNode->addAttribute("Key", ta::ToStringCast<ta::GroupGameDataKeyType>(groupGameDataKey.getKeyValue()));
			}

			{
				FVector spawnDataLocation = spawnData[spawnDataIndex]->GetActorLocation();

				childPositionNode->addAttribute("X", ta::ToStringCast<float>(spawnDataLocation.X));
				childPositionNode->addAttribute("Y", ta::ToStringCast<float>(spawnDataLocation.Y));
				childPositionNode->addAttribute("Z", ta::ToStringCast<float>(spawnDataLocation.Z));
			}

			{
				FRotator spawnDataRotation = spawnData[spawnDataIndex]->GetActorRotation();

				childRotationNode->addAttribute("X", ta::ToStringCast<float>(spawnDataRotation.Roll));
				childRotationNode->addAttribute("Y", ta::ToStringCast<float>(spawnDataRotation.Pitch));
				childRotationNode->addAttribute("Z", ta::ToStringCast<float>(spawnDataRotation.Yaw));
			}
		}

		if (false == ta::FileLoader::saveXml((ta::SpawnDataFilePath / (currentLevelName + "_" + currentFolderName)) += ".xml", &tempRoot))
		{
			TA_ASSERT_DEV(false, "save xml failed : %s %s", currentLevelName.c_str(), currentFolderName.c_str());
			return false;
		}
	}

	return true;
}


UTAGameInstance::UTAGameInstance()
{
	_navMeshExported = false;
	{
		//TA_LOG_DEV("%s", *FPaths::GetProjectFilePath());
		//TA_LOG_DEV("test log");
		//TA_LOG_DEV("%s", fs::current_path().c_str());
		
		// 스트링 에러남
		//const TCHAR* projectFilePath = *FPaths::GetProjectFilePath();

		ta::GameDataXmlFilePath = *FPaths::GetProjectFilePath();
		ta::GameDataXmlFilePath /= "../GameData/Xml";
		TA_LOG_DEV("%s", ta::GameDataXmlFilePath.c_str());

		ta::SpawnDataFilePath = *FPaths::GetProjectFilePath();
		ta::SpawnDataFilePath /= "../../TAGameServer/SpawnData";
		TA_LOG_DEV("%s", ta::SpawnDataFilePath.c_str());

		ta::NavigationMeshFilePath = *FPaths::GetProjectFilePath();
		ta::NavigationMeshFilePath /= "../../TAGameServer/NavigationMesh";
		TA_LOG_DEV("%s", ta::NavigationMeshFilePath.c_str());

		ta::PathPointFilePath = *FPaths::GetProjectFilePath();
		ta::PathPointFilePath /= "../../TAGameServer/PathPoint";
		TA_LOG_DEV("%s", ta::PathPointFilePath.c_str());
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
			TA_LOG_DEV("GameEventType : %d, ActorKey : %d", output->getGameEventType(), output->getActorKey().getKeyValue());
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
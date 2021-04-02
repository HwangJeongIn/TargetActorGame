// Fill out your copyright notice in the Description page of Project Settings.


#include "TAGameInstance.h"
//#include "Engine.h" 
//#include "Engine/Engine.h" 
#include "Engine/GameEngine.h" 
#include "TAGameMode.h"
#include "TANonPlayer.h"
#include "TAPlayer.h"
#include "TAPlayerController.h"
#include "TAPathPoint.h"
#include "TASpawnData.h"
#include "TAGameEvent.h"
#include "TimerManager.h"
#include "NavigationSystem.h"
#include "NavMesh/RecastNavMesh.h"
#include "Runtime/Engine/Public/EngineUtils.h"
#include "NavigationSystem.h"
#ifdef FOR_EDITING
#include "LevelEditor.h"
#include "Editor.h"
#endif
#include "Engine/LevelStreaming.h"
#include "TAAssets.h"
#include "Misc/App.h"

#include "Common/GameDataManager.h"
#include "Common/CommonSpawnDataManager.h"
#include "Common/CommonMoveActorSystem.h"
#include "Common/Serializer.h"
#include "Common/FileLoader.h"
#include "Common/GetComponentAndSystem.h"
#include "Common/StringUtility.h"
#include "Common/ScopedLock.h"
#include "Common/KeyDefinition.h"
#include "Common/GetComponentAndSystem.h"
#include "Common/CommonMoveActorComponent.h"
#include "Common/CommonCharacterActorComponent.h"
#include "Client/ClientActorManager.h"
#include "Client/ClientActor.h"
#include "Client/ClientApp.h"


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
#ifdef FOR_EDITING
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
#else
		TA_ASSERT_DEV(false, "비정상입니다.");
		return nullptr;
#endif
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

ATACharacter* TASpawnTAActor(const ta::ActorKey& actorKey, const FVector& position, const FRotator& rotation) noexcept
{
	UTAGameInstance* gameInstance = TAGetGameInstance();
	if (nullptr == gameInstance)
	{
		TA_ASSERT_DEV(false, "비정상입니다.");
		return nullptr;
	}

	return gameInstance->spawnTAActor(actorKey, position, rotation);
}

bool TADestroyTAActor(const ta::ActorKey& actorKey) noexcept
{
	UTAGameInstance* gameInstance = TAGetGameInstance();
	if (nullptr == gameInstance)
	{
		TA_ASSERT_DEV(false, "비정상입니다.");
		return nullptr;
	}

	return gameInstance->destroyTAActor(actorKey);
}

TWeakObjectPtr<ATACharacter> TAGetTAActor(const ta::ActorKey& actorKey) noexcept
{
	UTAGameInstance* gameInstance = TAGetGameInstance();
	if (nullptr == gameInstance)
	{
		TA_ASSERT_DEV(false, "비정상입니다.");
		return nullptr;
	}

	return gameInstance->getTAActor(actorKey);
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
	_findInteractionActorInterval = 1.5f;
	_currentFindInteractionActorTime = 0.0f;

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

	auto assets = GetDefault<UTAAssets>();
	//const uint32 skeletalMeshCount = assets->_skeletalMeshAssets.Num();
	//const uint32 staticMeshCount = assets->_staticMeshAssets.Num();
	//const uint32 animInstanceCount = assets->_animInstanceAssets.Num();

	{
		FString currentPath;
		uint32 count = assets->_skeletalMeshAssets.Num();
		TA_LOG_DEV("skeletal mesh asset count : %d", count);
		for (uint32 index = 0; index < count; ++index)
		{
			currentPath = assets->_skeletalMeshAssets[index].ToString();
			TA_LOG_DEV("skeletal mesh : %s", *currentPath);

			if (true == _skeletalMeshAssetKeyFinder.Contains(currentPath))
			{
				TA_ASSERT_DEV(false, "중복 파일입니다. %s", *currentPath);
				return;
			}

			_skeletalMeshAssetKeyFinder.Emplace(currentPath, index);
		}
	}

	{
		FString currentPath;
		uint32 count = assets->_staticMeshAssets.Num();
		TA_LOG_DEV("static mesh asset count : %d", count);
		for (uint32 index = 0; index < count; ++index)
		{
			currentPath = assets->_staticMeshAssets[index].ToString();
			TA_LOG_DEV("static mesh : %s", *currentPath);

			if (true == _staticMeshAssetKeyFinder.Contains(currentPath))
			{
				TA_ASSERT_DEV(false, "중복 파일입니다. %s", *currentPath);
				return;
			}

			_staticMeshAssetKeyFinder.Emplace(currentPath, index);
		}
	}

	{
		FString currentPath;
		uint32 count = assets->_animInstanceAssets.Num();
		TA_LOG_DEV("anim instance asset count : %d", count);
		for (uint32 index = 0; index < count; ++index)
		{
			currentPath = assets->_animInstanceAssets[index].ToString();
			TA_LOG_DEV("anim instance : %s", *currentPath);

			if (true == _animInstanceAssetKeyFinder.Contains(currentPath))
			{
				TA_ASSERT_DEV(false, "중복 파일입니다. %s", *currentPath);
				return;
			}

			_animInstanceAssetKeyFinder.Emplace(currentPath, index);
		}
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

	GetTimerManager().SetTimerForNextTick(this, &UTAGameInstance::processTimerTick);
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


void UTAGameInstance::processTimerTick(void) noexcept
{
	// 여러 스레드를 지원하지않는다고 문서에 나와있다. 데이터 레이스 문제는 걱정안해도 될 듯하다.
	GetTimerManager().SetTimerForNextTick(this, &UTAGameInstance::processTimerTick);

	const float deltaTime = GetWorld()->GetDeltaSeconds();
	
	// 완전히 동일하다.
	// TA_LOG_DEV("%f", GetWorld()->GetDeltaSeconds());
	// TA_LOG_DEV("%f", FApp::GetDeltaTime());
	
	processGameEventQueue(deltaTime);
	processFindInteractionActor(deltaTime);
}

void UTAGameInstance::processGameEventQueue(float deltaTime) noexcept
{
	//TA_LOG_DEV("processGameEvent");


	{
		ta::ScopedLock gameEventQueueLock(&_gameEventQueueLock);
		const uint32 gameEventCount = _gameEventQueue.size();
		if (0 == gameEventCount)
		{
			return;
		}

		TAGameEventProcessParameter parameter;
		parameter._gameInstance = this;
		parameter._world = GetWorld();

		for (uint32 index = 0; index < gameEventCount; ++index)
		{
			TAGameEvent* gameEvent = _gameEventQueue[index];
			if (nullptr != gameEvent)
			{
				//TA_LOG_DEV("GameEventType : %d, ActorKey : %d", gameEvent->getGameEventType(), gameEvent->getActorKey().getKeyValue());
				if (false == gameEvent->processEvent(parameter))
				{
					TA_ASSERT_DEV(false, "비정상입니다");
				}
				delete gameEvent;
			}
		}

		_gameEventQueue.clear();
	}

}

void UTAGameInstance::processFindInteractionActor(float deltaTime) noexcept
{
	_currentFindInteractionActorTime += deltaTime;

	if (_findInteractionActorInterval > _currentFindInteractionActorTime)
	{
		return;
	}

	_currentFindInteractionActorTime = 0.0f;

	ATAPlayerController* playerController = TAGetFirstPlayerController();
	ATAPlayer* player = Cast<ATAPlayer>(playerController->GetPawn());

	if (nullptr == player)
	{
		return;
	}

	if (true == playerController->getDialogVisibility())
	{
		return;
	}

	FVector playerPosition = player->GetActorLocation();
	const float maxInteractionDistanceSquared = 150.0f * 150.0f;
	float currentInteractionDistanceSquared = maxInteractionDistanceSquared;

	FVector interactionActorPosition;
	ta::ActorKey currentActorKey;
	ta::ActorKey interactionActorKey;
	ta::CommonActor* currentClientActor = nullptr;

	for (auto currentTAActor : _spawnedTAActors)
	{
		currentActorKey = currentTAActor.Key;
		// 있는지 검증 해야할까? => 어차피 요청했을때 없으면 실패하면 될듯
		// ta::CommonActor* currentClientActor = ta::GetActorManager()->getActor(currentActorKey);
		// if (nullptr == currentClientActor)
		// {
		// 	continue;
		// }

		// 비슷하고 거의 동기화 될것이기 때문에 그냥 UE4액터 위치로 계산하자
		//ta::CommonMoveActorComponent* actorMoveCom = ta::GetActorComponent<ta::CommonMoveActorComponent>(currentActorKey);
		//
		//ta::Vector interactionActorPosition;
		//{
		//	ta::ScopedLock moveComLock(actorMoveCom, true);
		//	interactionActorPosition = actorMoveCom->getCurrentPosition_();
		//}

		interactionActorPosition = currentTAActor.Value->GetActorLocation();
		float distanceSquared = (interactionActorPosition - playerPosition).SizeSquared();

		if (currentInteractionDistanceSquared > distanceSquared)
		{
			TA_LOG_DEV("processFindInteractionActor step1 ActorKey : %d, %f", currentTAActor.Key, distanceSquared);
			currentInteractionDistanceSquared = distanceSquared;
			interactionActorKey = currentTAActor.Key;
		}
	}

	_currentInteractionActorKey = interactionActorKey;

	if (true == _currentInteractionActorKey.isValid())
	{
		processInteractionMenu(playerController);
	}
	else
	{
		playerController->setInteractionMenuVisibility(false);
	}
}

void UTAGameInstance::processInteractionMenu(ATAPlayerController* playerController) noexcept
{
	TA_LOG_DEV("processInteractionMenu");

	if (false == _currentInteractionActorKey.isValid())
	{
		playerController->setInteractionMenuVisibility(false);
		return;
	}

	// 아래에서 컴포넌트 찾는것도 검증이 될 수 있어서 필요없음
	//ta::CommonActor* interactionClientActor = ta::GetActorManager()->getActor(_currentInteractionActorKey);
	//if (nullptr == interactionClientActor)
	//{
	//	_currentInteractionActorKey.clear();
	//	playerController->setInteractionMenuVisibility(false);
	//	return;
	//}

	ta::CommonCharacterActorComponent* characterCom = ta::GetActorComponent<ta::CommonCharacterActorComponent>(_currentInteractionActorKey);
	if (nullptr == characterCom)
	{
		_currentInteractionActorKey.clear();
		playerController->setInteractionMenuVisibility(false);
		return;
	}

	const ta::CharacterGameData* characterGameData = nullptr;
	// 한번 초기화되고 변할일이 없긴한데.. 그래도 일단 read 락걸고 가져오자
	{
		ta::ScopedLock characterLock(characterCom, true);
		characterGameData = characterCom->getCharacterGameData_();
	}

	const uint32 interactionTypeCount = characterGameData->_interactionTypes.size();
	{
		for (uint32 index = 0; index < interactionTypeCount; ++index)
		{
			// 버튼을 초기화
			const bool rv = playerController->setInteractionObjectsByInteractionType(_currentInteractionActorKey
																					 , static_cast<uint8>(index)
																					 , characterGameData->_interactionTypes[index]);

			if (false == rv)
			{
				TA_ASSERT_DEV(false, "비정상입니다");
				return;
			}
		}

		playerController->setVisibleButtons(interactionTypeCount);
		playerController->setInteractionMenuVisibility(true);
	}
}

bool UTAGameInstance::registerGameEvent(TAGameEvent* gameEvent) noexcept
{
	ta::ScopedLock gameEventQueueLock(&_gameEventQueueLock);
	_gameEventQueue.push_back(gameEvent);
	return true;
}

void UTAGameInstance::clearGameEvents(void) noexcept
{
	ta::ScopedLock gameEventQueueLock(&_gameEventQueueLock);
	const uint32 gameEventCount = _gameEventQueue.size();
	for (uint32 index = 0; index < gameEventCount; ++index)
	{
		if (nullptr != _gameEventQueue[index])
		{
			delete _gameEventQueue[index];
		}
	}

	_gameEventQueue.clear();
}

ATACharacter* UTAGameInstance::spawnTAActor(const ta::ActorKey& actorKey, const FVector& position, const FRotator& rotation) noexcept
{
	if (false == actorKey.isValid())
	{
		TA_ASSERT_DEV(false, "비정상입니다");
		return nullptr;
	}

	ta::ClientActor* actor = static_cast<ta::ClientActor*>(ta::GetActorManager()->getActor(actorKey));
	if (nullptr == actor)
	{
		TA_ASSERT_DEV(false, "비정상입니다.");
		return nullptr;
	}

	ATANonPlayer* character = nullptr;
	FActorSpawnParameters actorSpawnParameters;
	actorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	character = GetWorld()->SpawnActor<ATANonPlayer>(ATANonPlayer::StaticClass(), position, rotation, actorSpawnParameters);
	if (nullptr == character)
	{
		TA_ASSERT_DEV(false, "비정상입니다.");
		return nullptr;
	}

	if (true == _spawnedTAActors.Contains(actorKey.getKeyValue()))
	{
		TA_ASSERT_DEV(false, "두번 추가하려 합니다.");
		return nullptr;
	}
	
	_spawnedTAActors.Emplace(actorKey.getKeyValue(), character);

	character->setActorKey(actorKey);

	{
		ta::ScopedLock actorLock(actor);
		ATACharacter* tempCharacter = Cast<ATACharacter>(character);
		actor->setUnrealCharacter_(tempCharacter);
	}


	return character;
}

bool UTAGameInstance::destroyTAActor(const ta::ActorKey& actorKey) noexcept
{
	if (false == actorKey.isValid())
	{
		TA_ASSERT_DEV(false, "비정상입니다.");
		return false;
	}

	ta::ClientActor* actor = static_cast<ta::ClientActor*>(ta::GetActorManager()->getActor(actorKey));
	if (nullptr == actor)
	{
		TA_ASSERT_DEV(false, "비정상입니다.");
		return false;
	}

	ATANonPlayer** character = _spawnedTAActors.Find(actorKey.getKeyValue());
	if (nullptr == character)
	{
		TA_ASSERT_DEV(false, "해당 액터를 찾을 수 없습니다.");
		return false;
	}


	(*character)->resetActorKey();
	{
		ta::ScopedLock actorLock(actor);
		actor->resetUnrealCharacter_();
	}

	if (actorKey == _currentInteractionActorKey)
	{
		_currentInteractionActorKey.clear();
	}

	_spawnedTAActors.Remove(actorKey.getKeyValue());

	if (false == (*character)->ConditionalBeginDestroy())
	{
		TA_ASSERT_DEV(false, "비정상입니다.");
		return false;
	}

	return true;
}

TWeakObjectPtr<ATACharacter> UTAGameInstance::getTAActor(const ta::ActorKey& actorKey) noexcept
{
	if (false == actorKey.isValid())
	{
		TA_ASSERT_DEV(false, "비정상입니다.");
		return nullptr;
	}

	ATANonPlayer** character = _spawnedTAActors.Find(actorKey.getKeyValue());
	if (nullptr == character)
	{
		TA_ASSERT_DEV(false, "해당 액터를 찾을 수 없습니다.");
		return nullptr;
	}

	return *character;
}

FStreamableManager& UTAGameInstance::getStreamableManager(void) noexcept
{
	return _streamableManager;
}

FSoftObjectPath UTAGameInstance::getSkeletalMeshAssetPath(const FString& key) noexcept
{
	auto assets = GetDefault<UTAAssets>();

	uint32* targetIndex = _skeletalMeshAssetKeyFinder.Find(key);
	if (nullptr == targetIndex)
	{
		TA_ASSERT_DEV(false, "없습니다. %s", *key);
		return FSoftObjectPath();
	}

	return assets->_skeletalMeshAssets[*targetIndex];
}

FSoftObjectPath UTAGameInstance::getStaticMeshAssetPath(const FString& key) noexcept
{
	auto assets = GetDefault<UTAAssets>();

	uint32* targetIndex = _staticMeshAssetKeyFinder.Find(key);
	if (nullptr == targetIndex)
	{
		TA_ASSERT_DEV(false, "없습니다. %s", *key);
		return FSoftObjectPath();
	}

	return assets->_staticMeshAssets[*targetIndex];
}

FSoftObjectPath UTAGameInstance::getAnimInstanceAssetPath(const FString& key) noexcept
{
	auto assets = GetDefault<UTAAssets>();

	uint32* targetIndex = _animInstanceAssetKeyFinder.Find(key);
	if (nullptr == targetIndex)
	{
		TA_ASSERT_DEV(false, "없습니다. %s", *key);
		return FSoftObjectPath();
	}

	return assets->_animInstanceAssets[*targetIndex];
}

const ta::ActorKey& UTAGameInstance::getCurrentInteractionActorKey(void) noexcept
{
	// 있는지 검증
	ta::CommonActor* clientActor = ta::GetActorManager()->getActor(_currentInteractionActorKey);
	if (nullptr == clientActor)
	{
		_currentInteractionActorKey.clear();
	}

	return _currentInteractionActorKey;
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "TAExporter.h"
#include "Common/CommonBase.h"
#include "TANotSpawnedActor.h"
#include "TAGameInstance.h"
#include "Client/ClientMoveActorSystem.h"
#include "Common/GetComponentAndSystem.h"

// Sets default values
ATAExporter::ATAExporter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	_exportNavMesh = false;
	_exportPathPoint = false;
	_exportSpawnData = false;
}

// Called when the game starts or when spawned
void ATAExporter::BeginPlay()
{
	Super::BeginPlay();
	
}

#ifdef FOR_EDITING
void ATAExporter::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	TA_LOG_DEV("PropertyName : %s", *PropertyName.ToString());
	if (PropertyName == GET_MEMBER_NAME_CHECKED(ATAExporter, _exportNavMesh))
	{
		_exportNavMesh = false;
		TA_LOG_DEV("_exportNavMesh is changed");

		const bool result = TAExportRecastNavMesh();
		TA_LOG_DEV("ExportRecastNavMesh result : %d", result);
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(ATAExporter, _exportPathPoint))
	{
		_exportPathPoint = false;
		TA_LOG_DEV("_exportPathPoint is changed");

		const bool result = TAExportPathPoint();
		TA_LOG_DEV("ExportPathPoint result : %d", result);
		
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(ATAExporter, _exportSpawnData))
	{
		_exportSpawnData = false;
		TA_LOG_DEV("_exportSpawnData is changed");

		const bool result = TAExportSpawnData();
		TA_LOG_DEV("ExportSpawnData result : %d", result);
	}
	
}
#endif


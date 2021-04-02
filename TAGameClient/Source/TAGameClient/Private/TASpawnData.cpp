// Fill out your copyright notice in the Description page of Project Settings.


#include "TASpawnData.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Common/CommonBase.h"


// Sets default values
ATASpawnData::ATASpawnData()
{
	_groupGameDataKey = 0;

	// 크기에 대한 정보는 GameData에서 찾자 // 일단 배치할때는 발위치 기준으로
	_staticMesh->SetRelativeLocationAndRotation(FVector(0.f, 0.f, 0.f), FRotator(0.f, -90.f, 0.f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh>BoxMan(TEXT("/Game/_Dev/_Characters/InfinityBladeWarriors/Character/CompleteCharacters/BoxMan.BoxMan"));
	if (true == BoxMan.Succeeded())
	{
		_staticMesh->SetStaticMesh(BoxMan.Object);
	}
}

#ifdef FOR_EDITING
void ATASpawnData::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	TA_LOG_DEV("PropertyName : %s", *PropertyName.ToString());
	if (PropertyName == GET_MEMBER_NAME_CHECKED(ATASpawnData, _attachToTheGround))
	{
		// next가 바뀌었을때 해당 next에 접근해서 prev도 바꿔주자
		TA_LOG_DEV("_attachToTheGround is changed");

		_attachToTheGround = false;

		// ActorLineTraceSingle vs GetWorld()->LineTraceSingle
		// ActorLineTraceSingle : 액터컴포넌트를 호출하는 경우 작동
		// GetWorld()->LineTraceSingle : 전체월드

		UWorld* world = GetWorld();
		if (nullptr == world)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return;
		}

		const FVector startPos = GetActorLocation();
		const FVector endPos = startPos - (GetActorUpVector() * 1000);
		DrawDebugLine(world, startPos, endPos, FColor::Red, false, 1, 0, 5);
		
		FCollisionQueryParams traceParams(FName(TEXT("ToGroundTrace")), false, this);
		FHitResult hitResult;
		const bool rv = world->LineTraceSingleByChannel(hitResult
														, startPos
														, endPos
														, ECollisionChannel::ECC_WorldStatic
														, traceParams);

		if (false == rv)
		{
			TA_LOG_DEV("LineTraceSingleByChannel 실패");
			return;
		}

		if (false == hitResult.Actor.IsValid())
		{
			TA_LOG_DEV("LineTraceSingleByChannel 실패");
			return;
		}
		

		SetActorLocation(hitResult.Location);
	}
}
#endif

const int ATASpawnData::getGroupGameDataKey(void) const noexcept
{
	return _groupGameDataKey;
}
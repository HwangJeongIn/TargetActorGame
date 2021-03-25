// Fill out your copyright notice in the Description page of Project Settings.


#include "TASpawnData.h"
#include "DrawDebugHelpers.h"
#include "Common/CommonBase.h"


// Sets default values
ATASpawnData::ATASpawnData()
{
	_groupGameDataKey = 0;

	// 크기에 대한 정보는 GameData에서 찾자 // 일단 배치할때는 발위치 기준으로
	_staticMesh->SetRelativeLocationAndRotation(FVector(0.f, 0.f, 0.f), FRotator(0.f, -90.f, 0.f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh>BoxMan(TEXT("/Game/StaticMesh/BoxMan.BoxMan"));
	if (true == BoxMan.Succeeded())
	{
		_staticMesh->SetStaticMesh(BoxMan.Object);
	}
}

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

		FHitResult OutHit;
		FVector Start = GetActorLocation();

		Start.Z += 50.f;
		Start.X += 200.f;

		FVector ForwardVector = GetActorForwardVector();
		FVector End = ((ForwardVector * 500.f) + Start);
		FCollisionQueryParams CollisionParams;

		DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1, 0, 5);

		if (ActorLineTraceSingle(OutHit, Start, End, ECC_WorldStatic, CollisionParams))
		{
			//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::Printf(TEXT("The Component Being Hit is: %s"), *OutHit.GetComponent()->GetName()));
		}
	}
}
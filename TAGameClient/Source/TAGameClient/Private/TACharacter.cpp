// Fill out your copyright notice in the Description page of Project Settings.


#include "TACharacter.h"
#include "TAAnimInstance.h"
#include "TAPawn.h"
#include "TAGameInstance.h"
#include "TAAIController.h"
#include "Client/ClientApp.h"						  
#include "Client/ClientActor.h"			  
#include "Client/ClientActorManager.h"
#include "DrawDebugHelpers.h"


// Sets default values
ATACharacter::ATACharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// generate components
	USkeletalMeshComponent* mesh = nullptr;
	UCapsuleComponent* capsule = nullptr;
	{
		capsule = GetCapsuleComponent();
		mesh = GetMesh();
	}

	capsule->SetCapsuleHalfHeight(88.f);
	capsule->SetCapsuleRadius(34.0f);
	// 콜리전 프리셋 지정
	capsule->SetCollisionProfileName(TEXT("TACharacter"));
	mesh->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -88.f), FRotator(0.f, -90.f, 0.f));
	//GetCharacterMovement()->JumpZVelocity = 500.0f;
	
	//static ConstructorHelpers::FObjectFinder<USkeletalMesh> a(TEXT("/Game/_Dev/_Characters/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_FrostGiant.SK_CharM_FrostGiant"));


	//mesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	//static ConstructorHelpers::FClassFinder<UAnimInstance>WARRIOR_ANIM(TEXT("/Game/_Dev/_Characters/Animations/TAAnimationBlueprint.TAAnimationBlueprint_C"));
	//if (true == WARRIOR_ANIM.Succeeded())
	//{
	//	mesh->SetAnimInstanceClass(WARRIOR_ANIM.Class);
	//}
	//
	//static ConstructorHelpers::FObjectFinder<USkeletalMesh>SK_CARDBOARD(TEXT("/Game/_Dev/_Characters/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard"));
	//if (true == SK_CARDBOARD.Succeeded())
	//{
	//	mesh->SetSkeletalMesh(SK_CARDBOARD.Object);
	//}


	AIControllerClass = ATAAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	///////////////////////////////////////////////

	_actorKey.clear();
}

// Called every frame
void ATACharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ATACharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ATACharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

bool ATACharacter::setSkeletalMeshAndAnimInstance(const FString& skeletalMeshPath, const FString& animInstancePath) noexcept
{
	TA_LOG_DEV("setSkeletalMeshAndAnimInstance skeletalMeshPath : %s", *skeletalMeshPath);
	TA_LOG_DEV("setSkeletalMeshAndAnimInstance animaInstancePath : %s", *animInstancePath);


	_skeletalMeshPath = skeletalMeshPath;
	_animInstancePath = animInstancePath;

	UTAGameInstance* gameInstance = TAGetGameInstance();
	if (nullptr == gameInstance)
	{
		TA_ASSERT_DEV(false, "비정상");
		return false;
	}

	// 해당 에셋이 로드되었는지 검증하는 단계
	FSoftObjectPath skeletalMeshSoftObjectPath = gameInstance->getSkeletalMeshAssetPath(skeletalMeshPath);
	if (false == skeletalMeshSoftObjectPath.IsValid())
	{
		TA_ASSERT_DEV(false, "비정상");
		return false;
	}

	_skeletalMeshAssetStreamingHandle = gameInstance->getStreamableManager().RequestAsyncLoad(skeletalMeshSoftObjectPath
																							  , FStreamableDelegate::CreateUObject(this, &ATACharacter::onSkeletalMeshAssetLoadCompleted));

	return true;

	USkeletalMeshComponent* mesh = GetMesh();
	if (nullptr == mesh)
	{
		TA_ASSERT_DEV(false, "비정상");
		return false;
	}
	ConstructorHelpers::FObjectFinder<USkeletalMesh> targetSkeletalMesh(*skeletalMeshPath);
	if (false == targetSkeletalMesh.Succeeded())
	{
		TA_ASSERT_DEV(false, "비정상적인 경로 : %s", *skeletalMeshPath);
		return false;
	}
	mesh->SetSkeletalMesh(targetSkeletalMesh.Object);

	
	ConstructorHelpers::FClassFinder<UAnimInstance> targetAnimationBlueprint(*animInstancePath);
	if (false == targetAnimationBlueprint.Succeeded())
	{
		TA_ASSERT_DEV(false, "비정상적인 경로 : %s", *animInstancePath);
		return false;
	}
	mesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	mesh->SetAnimInstanceClass(targetAnimationBlueprint.Class);

}

void ATACharacter::onSkeletalMeshAssetLoadCompleted(void) noexcept
{
	TA_LOG_DEV("onSkeletalMeshAssetLoadCompleted");

	if (!_skeletalMeshAssetStreamingHandle)
	{
		TA_ASSERT_DEV(false, "비정상");
		return;
	}

	USkeletalMesh* asset = Cast<USkeletalMesh>(_skeletalMeshAssetStreamingHandle->GetLoadedAsset());
	_skeletalMeshAssetStreamingHandle.Reset();
	if (nullptr == asset)
	{
		TA_ASSERT_DEV(false, "비정상");
		return;
	}

	USkeletalMeshComponent* mesh = GetMesh();
	if (nullptr == mesh)
	{
		TA_ASSERT_DEV(false, "비정상");
		return;
	}
	mesh->SetSkeletalMesh(asset);

	// 이제 애니메이션 블루프린트를 불러온다.

	UTAGameInstance* gameInstance = TAGetGameInstance();
	if (nullptr == gameInstance)
	{
		TA_ASSERT_DEV(false, "비정상");
		return;
	}

	FSoftObjectPath animInstanceSoftObjectPath = gameInstance->getAnimInstanceAssetPath(_animInstancePath);
	if (false == animInstanceSoftObjectPath.IsValid())
	{
		TA_ASSERT_DEV(false, "비정상");
		return;
	}

	_animInstanceAssetStreamingHandle = gameInstance->getStreamableManager().RequestAsyncLoad(animInstanceSoftObjectPath
																							  , FStreamableDelegate::CreateUObject(this, &ATACharacter::onAnimInstanceAssetLoadCompleted));
}

void ATACharacter::onAnimInstanceAssetLoadCompleted(void) noexcept
{
	TA_LOG_DEV("onAnimInstanceAssetLoadCompleted");

	if (!_animInstanceAssetStreamingHandle)
	{
		TA_ASSERT_DEV(false, "비정상");
		return;
	}

	UAnimInstance* asset = Cast<UAnimInstance>(_animInstanceAssetStreamingHandle->GetLoadedAsset());
	_animInstanceAssetStreamingHandle.Reset();
	if (nullptr == asset)
	{
		TA_ASSERT_DEV(false, "비정상");
		return;
	}

	USkeletalMeshComponent* mesh = GetMesh();
	if (nullptr == mesh)
	{
		TA_ASSERT_DEV(false, "비정상");
		return;
	}
	mesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	mesh->SetAnimInstanceClass(asset->GetClass());
}

void ATACharacter::resetActorKey(void) noexcept
{
	_actorKey.clear();
}

bool ATACharacter::setActorKey(const ta::ActorKey& actorKey) noexcept
{
	if (false == actorKey.isValid())
	{
		TA_ASSERT_DEV(false, "비정상");
		return false;
	}

	if (true == _actorKey.isValid())
	{
		TA_ASSERT_DEV(false, "비정상");
		return false;
	}

	_actorKey = actorKey;
	return true;
}

const ta::ActorKey& ATACharacter::getActorKey(void) const noexcept
{
	return _actorKey;
}

ta::ClientActor* ATACharacter::getActorFromActorManager(void) const noexcept
{
	if (false == _actorKey.isValid())
	{
		TA_ASSERT_DEV(false, "비정상");
		return nullptr;
	}

	return static_cast<ta::ClientActor*>(ta::GetActorManager()->getActor(_actorKey));
}

void ATACharacter::setDeadAnimation(void) noexcept
{
	_animInstance->setDead(true);
	SetActorEnableCollision(false);
}

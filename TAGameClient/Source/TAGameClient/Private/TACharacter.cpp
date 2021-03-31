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

	setCharacterHeight(196.0f);

	// 콜리전 프리셋 지정
	capsule->SetCollisionProfileName(TEXT("TACharacter"));
	//GetCharacterMovement()->JumpZVelocity = 500.0f;
	
	//static ConstructorHelpers::FObjectFinder<USkeletalMesh> a(TEXT("/Game/_Dev/_Characters/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_FrostGiant.SK_CharM_FrostGiant"));


	//mesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	//static ConstructorHelpers::FClassFinder<UAnimInstance>WARRIOR_ANIM(TEXT("/Game/_Dev/_Characters/Animations/TAAnimationBlueprint.TAAnimationBlueprint_C"));
	//if (true == WARRIOR_ANIM.Succeeded())
	//{
	//	mesh->SetAnimInstanceClass(WARRIOR_ANIM.Class);
	//}
	//
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>SK_CARDBOARD(TEXT("/Game/_Dev/_Characters/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard"));
	if (true == SK_CARDBOARD.Succeeded())
	{
		mesh->SetSkeletalMesh(SK_CARDBOARD.Object);
	}

	_skeletalMeshPath.Empty();
	_animInstancePath.Empty();

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


void ATACharacter::BeginPlay()
{
	Super::BeginPlay();

	setCharacterHeight(196.0f);
	//TA_LOG_DEV("begin play ActorKey<%d>", getActorKey().getKeyValue());
	//if (true == _skeletalMeshPath.IsEmpty()
	//	|| true == _animInstancePath.IsEmpty())
	//{
	//	return;
	//}
	//
	//TA_LOG_DEV("begin play에서 setSkeletalMeshAndAnimInstance 실행 ActorKey<%d>", getActorKey().getKeyValue());
	//if (false == setSkeletalMeshAndAnimInstance(_skeletalMeshPath, _animInstancePath, false))
	//{
	//	TA_ASSERT_DEV(false, "비정상");
	//}
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

bool ATACharacter::setSkeletalMeshAndAnimInstance(const FString& skeletalMeshPath, const FString& animInstancePath/*, const bool reserveExecutionOnPlay*/) noexcept
{
	TA_LOG_DEV("setSkeletalMeshAndAnimInstance skeletalMeshPath : %s", *skeletalMeshPath);
	TA_LOG_DEV("setSkeletalMeshAndAnimInstance animaInstancePath : %s", *animInstancePath);


	_skeletalMeshPath = skeletalMeshPath;
	_animInstancePath = animInstancePath;

	//if (true == reserveExecutionOnPlay)
	//{
	//	TA_LOG_DEV("begin play에 실행하도록 예약되었습니다. ActorKey<%d>", getActorKey().getKeyValue());
	//	return true;
	//}

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
}

void ATACharacter::setCharacterHeight(const float height) noexcept
{
	USkeletalMeshComponent* mesh = GetMesh();
	UCapsuleComponent* capsule = GetCapsuleComponent();

	float halfHeight = (height / 2.0f);
	_characterHalfHeight = halfHeight;

	capsule->SetCapsuleSize(_characterHalfHeight * 0.35f, _characterHalfHeight);
	//capsule->SetCapsuleRadius(_characterHalfHeight * 0.35f, false); // 34
	//capsule->SetCapsuleHalfHeight(_characterHalfHeight, false);
	//capsule->SetCapsuleRadius(_characterHalfHeight * 0.35f, false); // 34

	mesh->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -_characterHalfHeight), FRotator(0.f, -90.f, 0.f));
}

float ATACharacter::getCharacterHalfHeight(void) const noexcept
{
	return _characterHalfHeight;
}

void ATACharacter::onSkeletalMeshAssetLoadCompleted() noexcept
{
	TA_LOG_DEV("onSkeletalMeshAssetLoadCompleted ActorKey<%d>, skeletalMeshPath : %s", getActorKey().getKeyValue(), *_skeletalMeshPath);

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
	bool directLoad = true;
	if (true == directLoad)
	{
		// 다이렉트로 클래스 로드해본다.
		UClass* animInstanceClass = LoadObject<UClass>(NULL, *_animInstancePath);
		if (nullptr == animInstanceClass)
		{
			TA_ASSERT_DEV(false, "로드 실패 %s", *_animInstancePath);
			return;
		}

		USkeletalMeshComponent* mesh = GetMesh();
		if (nullptr == mesh)
		{
			TA_ASSERT_DEV(false, "비정상");
			return;
		}
		mesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		mesh->SetAnimInstanceClass(animInstanceClass);
	}
	else // 아래 동작을 안한다. 혹시 모르니 일단 코드는 살려둔다.
	{
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
}

void ATACharacter::onAnimInstanceAssetLoadCompleted() noexcept
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

// Fill out your copyright notice in the Description page of Project Settings.


#include "TAPlayer.h"
#include "TAAnimInstance.h"
#include "TAGameInstance.h"
#include "TAAIController.h"
#include "TAPlayerController.h"
#include "DrawDebugHelpers.h"
#include "Common/GetComponentAndSystem.h"
#include "Client/ClientMoveActorSystem.h"


// Sets default values
ATAPlayer::ATAPlayer()
{
	PrimaryActorTick.bCanEverTick = true;

	// generate components
	USkeletalMeshComponent* mesh = nullptr;
	UCapsuleComponent* capsule = nullptr;
	{
		_springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINTARM"));
		_camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
		//_characterStat = CreateDefaultSubobject<UTPCharacterStatComponent>(TEXT("CHARACTERSTAT"));
		//_hpBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBARWIDGET"));
		capsule = GetCapsuleComponent();
		mesh = GetMesh();
	}

	// capsule  > mesh 
	//			> springArm > camera

	// set components

	_springArm->SetupAttachment(capsule);
	_camera->SetupAttachment(_springArm);
	_springArm->TargetArmLength = 400.f;
	_springArm->SetRelativeRotation(FRotator(-15.0f, 0.f, 0.f));

	_moveAndRotationDirtyFlag = false;
	_cameraDirtyFlag = false;
	_isAttacking = false;
	_maxCombo = 4;
	attackEndComboSate();

	setControlMode(ControlMode::PlayerFixedThirdPerson);

	// 따로 액터로 빠졌음
	//FName weaponSocket(TEXT("hand_rSocket"));
	//if (true == mesh->DoesSocketExist(weaponSocket))
	//{
	//	_weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WEAPON"));
	//	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_WEAPON(TEXT("/Game/InfinityBladeWeapons/Weapons/Blade/Swords/Blade_BlackKnight/SK_Blade_BlackKnight.SK_Blade_BlackKnight"));
	//	if (true == SK_WEAPON.Succeeded())
	//	{
	//		_weapon->SetSkeletalMesh(SK_WEAPON.Object);
	//	}

	//	_weapon->SetupAttachment(mesh, weaponSocket);
	//}

	//_currentWeapon = nullptr;

	//_hpBarWidget->SetupAttachment(mesh);
	//_hpBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));
	//_hpBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	//static ConstructorHelpers::FClassFinder<UUserWidget>UI_HPBAR(TEXT("/Game/Book/UI/UI_HPBar.UI_HPBar_C")); // 블루 프린트 => _C추가
	//if (true == UI_HPBAR.Succeeded())
	//{
	//	_hpBarWidget->SetWidgetClass(UI_HPBAR.Class);
	//	_hpBarWidget->SetDrawSize(FVector2D(150.0f, 50.0f));
	//}
	//// 생성되기 전에 가려준다.
	//_hpBarWidget->SetHiddenInGame(true);

	//// AI 컨트롤러 클래스 지정 // PlacedInWorldOrSpawned : 배치되거나 스폰되는 것들에 대해서 적용
	//AIControllerClass = ATAAIController::StaticClass();
	//AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// Called when the game starts or when spawned
void ATAPlayer::BeginPlay()
{
	Super::BeginPlay();
	//GetTAGameInstance()->exportNavMesh();
}

// Called every frame
void ATAPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//TA_LOG_DEV("ATAPlayer::Tick");

	//if (nullptr == _target.Get())
	//{
	//	TA_LOG_DEV("nullptr");
	//}
	//
	////if (false == _target->IsValidLowLevel())
	////{
	////	TA_LOG_DEV("damgling ptr");
	////}
	//
	//if(true == _target.IsValid())
	//{
	//	TA_LOG_DEV("valid ptr");
	//}
	//else
	//{
	//	TA_LOG_DEV("invalid ptr");
	//}

	AController* controller = GetController();
	/*

	static float time = 0.0f;
	time += DeltaTime;
	
	if (time > 2.0f)
	{
		TA_LOG_DEV("Controller_pl");
		time = 0.0f;
		AAIController* test1 = Cast<AAIController>(controller);
		APlayerController* test2 = Cast<APlayerController>(controller);

		if (nullptr != test1)
		{
			TA_LOG_DEV("AIController1_pl");
			test1->MoveToLocation(GetActorLocation() + FVector(200, 0, 0), 1.0f);
		}

		if (nullptr != test2)
		{
			TA_LOG_DEV("PlayerController_pl");

			SetActorLocation(GetActorLocation() + FVector(200, 0, 10));
		}
	}
	*/

	// 보간작업
	if (true == _cameraDirtyFlag)
	{
		bool armLengthUpdateComplete = false;
		if (_springArm->TargetArmLength != _toArmLength)
		{
			_springArm->TargetArmLength = FMath::FInterpTo(_springArm->TargetArmLength, _toArmLength, DeltaTime, _armLengthSpeed);
		}
		else
		{
			armLengthUpdateComplete = true;
		}

		const FRotator relativeRotation = _springArm->GetRelativeRotation();
		if (relativeRotation != _toArmRotation)
		{
			_springArm->SetRelativeRotation(FMath::RInterpTo(relativeRotation, _toArmRotation, DeltaTime, _armRotationSpeed));
		}
		else
		{
			if (true == armLengthUpdateComplete)
			{
				_cameraDirtyFlag = false;
			}
		}
	}

	if (true == _moveAndRotationDirtyFlag)
	{
		switch (_currentControlMode)
		{
		case ControlMode::PlayerThirdPerson:
			{
				// do nothing
			}
			break;
		case ControlMode::PlayerFixedThirdPerson:
			{
				if ((0.0f < _toMoveDirectionByInput.SizeSquared())
					&& (nullptr != controller))
				{
					//TA_LOG_DEV("direction to move(%f,%f,%f)", _toMoveDirectionByInput.X, _toMoveDirectionByInput.Y, _toMoveDirectionByInput.Z);

					// MakeFromX : 회전행렬 생성하고 거기에 Rotator를 가지고 온다 // 단 여기 매개변수의 값의 z값이 1.0f - 0.0001보다 크면 이상하게 나온다
					// SetControlRotation : 내부 컨트롤 로테이션을 변경한다. // AddControllerYawInput같은 것과는 다른듯 // AddControllerYawInput은 카메라도 돌려줌
					controller->SetControlRotation(FRotationMatrix::MakeFromX(_toMoveDirectionByInput).Rotator());

					// 폰내부 movement component에 의해 움직임
					//AddMovementInput(_toMoveDirectionByInput);
					//TPLOG(Warning, TEXT("%f"), KINDA_SMALL_NUMBER);
				}
			}
			break;
		default:
			{
				TA_ASSERT_DEV(false, "정의되지 않은 타입입니다.");
			}
			break;
		}
		_moveAndRotationDirtyFlag = false;
	}
}

// Called to bind functionality to input
void ATAPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &ATAPlayer::upDown);
	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &ATAPlayer::leftRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ATAPlayer::lookUp);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ATAPlayer::turn);

	PlayerInputComponent->BindAction(TEXT("ViewChange"), EInputEvent::IE_Pressed, this, &ATAPlayer::viewChange);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &ATAPlayer::attack);
	PlayerInputComponent->BindAction(TEXT("ToggleInventory"), EInputEvent::IE_Pressed, this, &ATAPlayer::toggleInventory);
	PlayerInputComponent->BindAction(TEXT("ToggleMousePoint"), EInputEvent::IE_Pressed, this, &ATAPlayer::toggleMousePoint);

	// temp
	PlayerInputComponent->BindAction(TEXT("ExportRecastNavMesh"), EInputEvent::IE_Pressed, this, &ATAPlayer::exportRecastNavMesh);
}

void ATAPlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	_animInstance = Cast<UTAAnimInstance>(GetMesh()->GetAnimInstance());
	TA_ASSERT_DEV(nullptr != _animInstance, "AnimInstance를 받아올 수 없습니다.");

	// 델리게이트에 연결시킨다. // 멀티캐스트(여러 함수가 등록됨)

	// 애니메이션 관련
	{
		_animInstance->OnMontageEnded.AddDynamic(this, &ATAPlayer::onAttackMontageEnded);

		// // 타격이 들어갔을때 델리게이트에 람다식 연결 // 세부 컨트롤 하고 싶으면 여기서 해주면될듯 // 그 애니메이션 특정 구간에서만 가능하게하려면
		//_animInstance->_onAttackHitCheck.AddLambda(
		//	[this]()->void
		//	{
		//		TPLOG_DEV("OnAttackHitCheck");
		//		_canNextCombo = true;
		//	}
		//);

		// 다음 콤보에 대한 델리게이트에 람다식 연결
		_animInstance->_onNextAttackCheck.AddLambda(
			[this]()->void
			{

				_canNextCombo = false;
				if (true == _isComboInputOn)
				{
					TA_LOG_DEV("NextAttack");
					attackStartComboSate();
					_animInstance->jumpToAttackMontageSection(_currentCombo);
				}
			}
		);

		_animInstance->_onAttackHitCheck.AddUObject(this, &ATAPlayer::attackCheck);
	}

}

void ATAPlayer::moveAndRotateCharacterByInput(const CharacterBehaviorByInput& characterBehaviorByInput, const DirectionType& directionType, const float axisValue) noexcept
{
	if (0.0f == axisValue)
	{
		if (CharacterBehaviorByInput::Move != characterBehaviorByInput)
		{
			return;
		}

		switch (directionType)
		{
		case DirectionType::X:
			{
				_toMoveDirectionByInput.X = 0.0f;
			}
			break;
		case DirectionType::Y:
			{
				_toMoveDirectionByInput.Y = 0.0f;
			}
			break;
		case DirectionType::Z:
			{
				_toMoveDirectionByInput.Z = 0.0f;
			}
			break;
		default:
			{
				TA_ASSERT_DEV(false, "정의되지 않은 타입입니다.");
			}
			break;
		}
		return;
	}


	if (CharacterBehaviorByInput::Move == characterBehaviorByInput)
	{
		moveCharacterByInput(directionType, axisValue);
	}
	else if (CharacterBehaviorByInput::Rotate == characterBehaviorByInput)
	{
		rotateCharacterByInput(directionType, axisValue);
	}

	_moveAndRotationDirtyFlag = true;
}

void ATAPlayer::moveCharacterByInput(const DirectionType& directionType, const float axisValue) noexcept
{
	switch (_currentControlMode)
	{
	case ControlMode::PlayerThirdPerson:
		{
			FVector direction = FVector::ZeroVector;
			switch (directionType)
			{
				// transform 행렬의 각행을 가지고 온것들
			case DirectionType::X:
				{
					direction = FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::X);
				}
				break;
			case DirectionType::Y:
				{
					direction = FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::Y);
				}
				break;
			case DirectionType::Z:
				{
					direction = FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::Z);
				}
				break;
			default:
				{
					TA_ASSERT_DEV(false, "정의되지 않은 타입입니다.");
				}
				break;
			}
			AddMovementInput(direction, axisValue);
		}
		break;
	case ControlMode::PlayerFixedThirdPerson:
		{
			switch (directionType)
			{
			case DirectionType::X:
				{
					_toMoveDirectionByInput.X = axisValue;
				}
				break;
			case DirectionType::Y:
				{
					_toMoveDirectionByInput.Y = axisValue;
				}
				break;
			case DirectionType::Z:
				{
					_toMoveDirectionByInput.Z = axisValue;
				}
				break;
			default:
				{
					TA_ASSERT_DEV(false, "정의되지 않은 타입입니다.");
				}
				break;
			}
			//TA_LOG_DEV("direction to move(%f,%f,%f)", _toMoveDirectionByInput.X, _toMoveDirectionByInput.Y, _toMoveDirectionByInput.Z);
			AddMovementInput(_toMoveDirectionByInput);
		}
		break;
	default:
		{
			TA_ASSERT_DEV(false, "정의되지 않은 타입입니다.");
		}
		break;
	}

}

void ATAPlayer::rotateCharacterByInput(const DirectionType& directionType, const float axisValue) noexcept
{
	switch (_currentControlMode)
	{
	case ControlMode::PlayerThirdPerson:
		{
			switch (directionType)
			{
			case DirectionType::X:
				{

				}
				break;
			case DirectionType::Y:
				{
					// PlayerControl은 카메라랑 연동되어있음 카메라 로테이션 보간작업이 내부에서 이루어짐
					// PlayerControl안에 있는 RotationInput을 변경
					// 컨트롤 로테이션 , 폰 로테이션 갱신해줌
					// 폰아래에 카메라가 있는데 그 카메라가 움직이는 이유는 로테이션 bUsePawnControlRotation, 상속 플래그를 켜놔서 그렇다 // 끄면 카메라는 움직이지 않는다.
					AddControllerPitchInput(axisValue);
				}
				break;
			case DirectionType::Z:
				{
					// PlayerControl안에 있는 RotationInput을 변경
					AddControllerYawInput(axisValue);
				}
				break;
			default:
				{
					TA_ASSERT_DEV(false, "정의되지 않은 타입입니다.");
				}
				break;
			}
		}
		break;
	case ControlMode::PlayerFixedThirdPerson: // 카메라 안움직이는데 컨트롤러만 움직여놓으면 더 헷갈린다. // 그냥 키보드로 방향정해서 움직이게 하자
		{
			// do nothing
		}
		break;
	default:
		{
			TA_ASSERT_DEV(false, "정의되지 않은 타입입니다.");
		}
		break;
	}
}

void ATAPlayer::upDown(const float axisValue) noexcept
{
	moveAndRotateCharacterByInput(CharacterBehaviorByInput::Move, DirectionType::X, axisValue);
}

void ATAPlayer::leftRight(const float axisValue) noexcept
{
	moveAndRotateCharacterByInput(CharacterBehaviorByInput::Move, DirectionType::Y, axisValue);
}

void ATAPlayer::lookUp(const float axisValue) noexcept
{
	moveAndRotateCharacterByInput(CharacterBehaviorByInput::Rotate, DirectionType::Y, axisValue);
}

void ATAPlayer::turn(const float axisValue) noexcept
{
	moveAndRotateCharacterByInput(CharacterBehaviorByInput::Rotate, DirectionType::Z, axisValue);
}

void ATAPlayer::attack(void) noexcept
{
	if (true == _isAttacking)
	{
		if (false == FMath::IsWithinInclusive<int32>(_currentCombo, 1, _maxCombo))
		{
			TA_ASSERT_DEV(false, "현재 콤보수가 비정상입니다.");
			return;
		}

		if (true == _canNextCombo)
		{
			TA_LOG_DEV("combo input");
			_isComboInputOn = true;
		}
	}
	else
	{
		TA_ASSERT_DEV(0 == _currentCombo, "처음 공격을 시작하는데 콤보수가 0이 아닙니다. 확인해야합니다.");
		attackStartComboSate();
		if (nullptr != _animInstance)
		{
			_animInstance->playAttackMontage();
			_animInstance->jumpToAttackMontageSection(_currentCombo);
		}
		_isAttacking = true;
	}
}

void ATAPlayer::toggleInventory(void) noexcept
{
	ATAPlayerController* controller = static_cast<ATAPlayerController*>(GetController());
	const bool toggleFlag = !(controller->getInventoryVisibility());
	controller->bShowMouseCursor = toggleFlag;
	controller->setInventoryVisibility(toggleFlag);
	TA_LOG_DEV("Toggle inventory : %d", toggleFlag);
}

void ATAPlayer::toggleMousePoint(void) noexcept
{
	ATAPlayerController* controller = static_cast<ATAPlayerController*>(GetController());
	controller->bShowMouseCursor = !(controller->bShowMouseCursor);
	TA_LOG_DEV("Toggle mouse point : %d", controller->bShowMouseCursor);
}

void ATAPlayer::exportRecastNavMesh(void) noexcept
{
	ta::ClientMoveActorSystem* clientMoveActorSystem = ta::GetActorSystem<ta::ClientMoveActorSystem>();
	const bool result = clientMoveActorSystem->exportRecastNavMesh();
	TA_LOG_DEV("ExportRecastNavMesh result : %d", result);
}

void ATAPlayer::onAttackMontageEnded(UAnimMontage* montage, bool interrupted) noexcept
{
	TA_ASSERT_DEV(true == _isAttacking, "변수값이 비정상입니다. isAttacking : %d", _isAttacking);
	TA_ASSERT_DEV(0 < _currentCombo, "종료시 콤보값이 비정상입니다. : %d", _currentCombo);
	TA_LOG_DEV("onAttackMontageEnded");
	_isAttacking = false;
	attackEndComboSate();

	//_onAttackEnd.Broadcast();
}

void ATAPlayer::attackStartComboSate(void) noexcept
{
	_isComboInputOn = false;
	_canNextCombo = true;
	TA_ASSERT_DEV(FMath::IsWithinInclusive<int32>(_currentCombo, 0, _maxCombo - 1), "현재 콤보수가 비정상입니다. : %d", _currentCombo);
	_currentCombo = FMath::Clamp<int32>(_currentCombo + 1, 1, _maxCombo);
}

void ATAPlayer::attackEndComboSate(void) noexcept
{
	_isComboInputOn = false;
	_canNextCombo = false;
	_currentCombo = 0;
}

void ATAPlayer::attackCheck(void) noexcept
{
	// 바꿔야한다.
	const float finalAttackRange = 200.0f;
	const float finalAttackRadius = 50.0f;

	interact(InteractType::Attack, finalAttackRange, finalAttackRadius);
}

void ATAPlayer::interact(const InteractType& interactType, float range, float radius) noexcept
{
	FHitResult hitResult;
	FCollisionQueryParams params(NAME_None, false, this);
	const FVector actorPosition = GetActorLocation();
	const FVector attackRangeWithDirection = GetActorForwardVector() * range;

	bool result = GetWorld()->SweepSingleByChannel(hitResult
												   , actorPosition // 액터위치에서
												   , actorPosition + attackRangeWithDirection // 앞으로 + Range까지
												   , FQuat::Identity // 회전 x
												   , ECollisionChannel::ECC_GameTraceChannel2 // config/defaultEngine.ini 확인결과 ECC_GameTraceChannel1 : TPCharacter , ECC_GameTraceChannel2 : Attack
												   , FCollisionShape::MakeSphere(radius)
												   , params);
#define ENABLE_DRAW_DEBUG
#ifdef ENABLE_DRAW_DEBUG

	const FVector traceRangeCenter = actorPosition + attackRangeWithDirection * 0.5f;
	const float halfLength = range * 0.5f + radius;
	FQuat capsuleRot = FRotationMatrix::MakeFromZ(attackRangeWithDirection).ToQuat();
	FColor drawColor = true == result ? FColor::Green : FColor::Red;
	float debugLifeTime = 5.0f;

	DrawDebugCapsule(GetWorld()
					 , traceRangeCenter
					 , halfLength
					 , radius
					 , capsuleRot
					 , drawColor
					 , false
					 , debugLifeTime);

#endif
#undef ENABLE_DRAW_DEBUG

	if (true == result)
	{
		if (false == hitResult.Actor.IsValid())
		{
			return;
		}

		{
			if (false == doInteract(hitResult, interactType))
			{
				TA_ASSERT_DEV(false, "DoInteract에 실패했습니다.");
				return;
			}

			if (false == postInteract(hitResult, interactType))
			{
				TA_ASSERT_DEV(false, "PostInteract에 실패했습니다.");
				return;
			}
		}
	}
}

bool ATAPlayer::doInteract(FHitResult& hitResult, const InteractType& interactType) noexcept
{
	if (false == hitResult.Actor.IsValid())
	{
		TA_ASSERT_DEV(false, "코드상 있을 수 없는 상황");
		return false;
	}

	switch (interactType)
	{
	case InteractType::Attack:
		{
			TA_LOG_DEV("Hit ActorName : %s", *hitResult.Actor->GetName());

			// 테스트
			//ATAPlayer* character = Cast<ATAPlayer>(hitResult.Actor);
			//if (nullptr != character)
			//{
			//	character->setDeadAnimation();
			//}
			//FDamageEvent damageEvent;
			//hitResult.Actor->TakeDamage(finalAttackRange, damageEvent, GetController(), this);
		}
		break;
	case InteractType::Gather:
		{
			// 추가해야한다.
		}
		break;
	default:
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}
		break;
		TA_COMPILE_DEV(2 == static_cast<ta::uint8>(InteractType::Count), "여기도 확인해주세요");
	}

	return true;
}

bool ATAPlayer::postInteract(FHitResult& hitResult, const InteractType& interactType) noexcept
{
	if (false == hitResult.Actor.IsValid())
	{
		TA_ASSERT_DEV(false, "코드상 있을 수 없는 상황");
		return false;
	}

	switch (interactType)
	{
	case InteractType::Attack:
		{
			// 추가해야한다.
		}
		break;
	case InteractType::Gather:
		{
			// 추가해야한다.
		}
		break;
	default:
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}
		break;
		TA_COMPILE_DEV(2 == static_cast<ta::uint8>(InteractType::Count), "여기도 확인해주세요");
	}

	return true;
}

void ATAPlayer::setControlMode(const ControlMode controlMode) noexcept
{
	// UCharacterMovementComponent : 캐릭터 움직임 관리
	UCharacterMovementComponent* characterMovement = GetCharacterMovement();

	switch (controlMode)
	{
	case ControlMode::PlayerThirdPerson:
		{
			//_springArm->TargetArmLength = 450.f;
			//_springArm->SetRelativeRotation(FRotator::ZeroRotator);
			_toArmLength = 450.0f;

			// 폰은 안따라가는데 컨트롤러 로테이션 카메라만 따라간다. 폰이 컨트롤러 로테이션 기준으로 움직이면 거기에 맞게 로테이션 

			// GetViewRotation = GetControlRotation
			_springArm->bUsePawnControlRotation = true; // 이플래그 사용시 자체회전 x 부모의 회전(pawn's controlRotation)을 받아옴
			_springArm->bInheritPitch = true; // 해당 회전값을 부모로 부터 받을지 결정
			_springArm->bInheritRoll = true;
			_springArm->bInheritYaw = true;
			_springArm->bDoCollisionTest = true;

			// 이렇게 해버리면 컨트롤러만 로테이션이 적용 // 아래에서 따로 처리함
			// 카메라가 보는 방향으로 바로 캐릭터가 이동하지 않고 방향키를 눌러야 회전 > 이동
			bUseControllerRotationYaw = false;

			// 여기서 자동으로 회전시켜주는 movement component 설정
			if (nullptr != characterMovement)
			{
				characterMovement->bOrientRotationToMovement = true;
				//characterMovement->bUseControllerDesiredRotation = false;
				characterMovement->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
			}
		}
		break;
	case ControlMode::PlayerFixedThirdPerson:
		{
			//_springArm->TargetArmLength = 800.f;
			//_springArm->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));
			_toArmLength = 800.0f;
			_toArmRotation = FRotator(-45.0f, 0.0f, 0.0f);

			_springArm->bUsePawnControlRotation = false; // 부모의 영향(pawn's controlRotation)을 받지 않음
			// 부모의 영향을 받지 않는다.
			_springArm->bInheritPitch = false;
			_springArm->bInheritRoll = false;
			_springArm->bInheritYaw = false;
			// 레벨에 잘릴 수 있다 // 거리때문에 상관없을듯
			_springArm->bDoCollisionTest = false;
			bUseControllerRotationYaw = false; // bUseControllerDesiredRotation이걸로 보간해줘서 false로 해야한다. true로하면 끊긴다.

			if (nullptr != characterMovement)
			{
				//characterMovement->bOrientRotationToMovement = true;
				//characterMovement->bUseControllerDesiredRotation = false;

				characterMovement->bOrientRotationToMovement = false;
				characterMovement->bUseControllerDesiredRotation = true;
				characterMovement->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
			}
		}
		break;
	default:
		{
			TA_ASSERT_DEV(false, "정의되지 않은 타입입니다.");
		}
		break;
	}

	_currentControlMode = controlMode;
	_cameraDirtyFlag = true;
}

void ATAPlayer::viewChange(void) noexcept
{
	switch (_currentControlMode)
	{
	case ControlMode::PlayerThirdPerson:
		{
			//_toArmRotation = GetActorRotation();

			// GetActorRotation : 액터의 루트 컴포넌트의 로테이션 
			// PlayerThirdPerson모드에서 마우스를 움직일때 변경 : Controll , Pawn 로테이션(액터의 루트 컴포넌트의 로테이션)
			_springArm->SetRelativeRotation(GetController()->GetControlRotation());
			// 원래 pawn's control rotation을 사용하고 있었다 > 하지만 이제 사용안할것이기 때문에 현재의 절대값의 컨트롤 로테이션을 받아서 보간을 준비한다.
			// 만약 여기서 이처리를 해주지 않으면 상대값으로 사용하고 있던 카메라 로테이션값이 절대값으로 해석되면서 카메라가 튀게된다. 
			GetController()->SetControlRotation(GetActorRotation()); // PlayerThirdPerson => PlayerFixedThirdPerson // 이제 컨트롤 로테이션값은 화면의 의미가 없어지고 오직 액터의 방향만 나타낸다 // 액터기준으로 한번 세팅해준다.
			setControlMode(ControlMode::PlayerFixedThirdPerson);
		}
		break;
	case ControlMode::PlayerFixedThirdPerson:
		{
			//_toArmRotation = _springArm->GetRelativeRotation();
			// 현재 카메라가 보는 시점기준으로 한번세팅한다 // 이제 컨트롤러에 의해 카메라가 통제되기 때문에 싱크를 맞추기 위해서 카메라위치에 컨트롤러를 배치한다
			GetController()->SetControlRotation(_springArm->GetRelativeRotation()); // PlayerFixedThirdPerson => PlayerThirdPerson
			setControlMode(ControlMode::PlayerThirdPerson);
		}
		break;
	default:
		break;
	}
}

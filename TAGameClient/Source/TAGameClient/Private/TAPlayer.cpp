// Fill out your copyright notice in the Description page of Project Settings.


#include "TAPlayer.h"
#include "TAAnimInstance.h"
#include "TAGameInstance.h"
#include "TAAIController.h"
#include "TAPlayerController.h"
#include "TAConvertFunctions.h"
#include "TAInteractionButtonUserWidget.h"
#include "DrawDebugHelpers.h"
#include "Common/GetComponentAndSystem.h"
#include "Common/ScopedLock.h"
#include "Client/ClientMoveActorSystem.h"
#include "Client/ClientMoveActorComponent.h"
#include "Client/ClientActor.h"


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

	_thirdPersonToArmLength = 1350.0f;
	_fixedThirdPersonToArmLength = 2700.0f;
	_focusedAndControlBlockedToArmLength = 300.0f;
	_fixedThirdPersonRotator = FRotator(-45.0f, 0.0f, 0.0f);

	_armLengthSpeed = 5.0f;
	_armRotationSpeed = 5.0f;
	_armPositionSpeed = 5.0f;


	_moveAndRotationDirtyFlag = false;
	_cameraDirtyFlag = false;
	_isAttacking = false;
	_maxCombo = 4;
	attackEndComboSate();

	_syncInterval = 1.0f;
	_currentTimeForSync = 0.0f;

	setControlMode(ControlMode::PlayerThirdPerson);
	_focusedCharacter = this;
}

// Called when the game starts or when spawned
void ATAPlayer::BeginPlay()
{
	Super::BeginPlay();
	if (false == setSkeletalMeshAndAnimInstance("/Game/_Dev/_Characters/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard"
		, "/Game/_Dev/_Characters/Animations/TAAnimationBlueprint.TAAnimationBlueprint_C"))
	
	//if (false == setSkeletalMeshAndAnimInstance("/Game/_Dev/_Characters/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard"
	//	, "/Game/_Dev/_Characters/Animations/TAAnimationBlueprint.TAAnimationBlueprint_C"))
	{
		TA_ASSERT_DEV(false, "비정상");
	}

}

// Called every frame
void ATAPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	processSyncToServer(DeltaTime);

	AController* controller = GetController();
	
	// 카메라 보간작업 // 추후 더티플래그 비트로 관리해야할듯
	if (true == _cameraDirtyFlag)
	{
		bool armLengthUpdateComplete = false;
		if (1.0f < FMath::Abs(_springArm->TargetArmLength - _toArmLength))
		{
			_springArm->TargetArmLength = FMath::FInterpTo(_springArm->TargetArmLength, _toArmLength, DeltaTime, _armLengthSpeed);
		}
		else
		{
			armLengthUpdateComplete = true;
		}

		const FRotator relativeRotation = _springArm->GetRelativeRotation();
		bool armRelativeRotationUpdateComplete = false;
		if (false == (_toArmRelativeRotation - relativeRotation).IsNearlyZero())
		{
			_springArm->SetRelativeRotation(FMath::RInterpTo(relativeRotation, _toArmRelativeRotation, DeltaTime, _armRotationSpeed));
		}
		else
		{
			armRelativeRotationUpdateComplete = true;
		}

		const FVector relativePosition = _springArm->GetRelativeLocation();
		bool armRelativePositionUpdateComplete = false;
		if (false == (_toArmRelativePosition - relativePosition).IsNearlyZero())
		{
			_springArm->SetRelativeLocation(FMath::VInterpTo(relativePosition, _toArmRelativePosition, DeltaTime, _armRotationSpeed));
		}
		else
		{
			armRelativePositionUpdateComplete = true;
		}

		if (true == armLengthUpdateComplete
			&& true == armRelativeRotationUpdateComplete
			&& true == armRelativePositionUpdateComplete)
		{
			TA_LOG_DEV("(interp to) => complete");
			_cameraDirtyFlag = false;
			onViewChangedComplete();
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

	// for test
	PlayerInputComponent->BindAction(TEXT("Key1Pressed"), EInputEvent::IE_Pressed, this, &ATAPlayer::key1Pressed);
	PlayerInputComponent->BindAction(TEXT("Key2Pressed"), EInputEvent::IE_Pressed, this, &ATAPlayer::key2Pressed);
	PlayerInputComponent->BindAction(TEXT("Key3Pressed"), EInputEvent::IE_Pressed, this, &ATAPlayer::key3Pressed);

	// for interaction button
	PlayerInputComponent->BindAction(TEXT("Interaction1KeyPressed"), EInputEvent::IE_Pressed, this, &ATAPlayer::interaction1KeyPressed);
	PlayerInputComponent->BindAction(TEXT("Interaction2KeyPressed"), EInputEvent::IE_Pressed, this, &ATAPlayer::interaction2KeyPressed);
	PlayerInputComponent->BindAction(TEXT("Interaction3KeyPressed"), EInputEvent::IE_Pressed, this, &ATAPlayer::interaction3KeyPressed);
}

void ATAPlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	_animInstance = Cast<UTAAnimInstance>(GetMesh()->GetAnimInstance());
	if (nullptr == _animInstance)
	{
		TA_ASSERT_DEV(false, "AnimInstance를 받아올 수 없습니다.");
		return;
	}

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
		_animInstance->_onNextAttackCheck.AddDynamic(this, &ATAPlayer::nextAttackCheck);
		_animInstance->_onAttackHitCheck.AddDynamic(this, &ATAPlayer::attackCheck);
	}

}

void ATAPlayer::moveAndRotateCharacterByInput(const CharacterBehaviorByInput& characterBehaviorByInput, const DirectionType& directionType, const float axisValue) noexcept
{
	if (ControlMode::FocusedAndControlBlocked == _currentControlMode)
	{
		return;
	}

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

void ATAPlayer::attack() noexcept
{
	if (ControlMode::FocusedAndControlBlocked == _currentControlMode)
	{
		return;
	}

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

void ATAPlayer::attackCheck() noexcept
{
	// 바꿔야한다.
	const float finalAttackRange = 200.0f;
	const float finalAttackRadius = 50.0f;

	interact(ta::InteractionType::Attack, finalAttackRange, finalAttackRadius);
}

void ATAPlayer::nextAttackCheck() noexcept
{
	_canNextCombo = false;
	if (true == _isComboInputOn)
	{
		TA_LOG_DEV("NextAttack");
		attackStartComboSate();
		_animInstance->jumpToAttackMontageSection(_currentCombo);
	}
}

void ATAPlayer::interact(const ta::InteractionType& interactionType, float range, float radius) noexcept
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
#define _ENABLE_DRAW_DEBUG
#ifdef _ENABLE_DRAW_DEBUG

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
#undef _ENABLE_DRAW_DEBUG

	if (true == result)
	{
		if (false == hitResult.Actor.IsValid())
		{
			return;
		}

		{
			if (false == doInteract(hitResult, interactionType))
			{
				TA_ASSERT_DEV(false, "DoInteract에 실패했습니다.");
				return;
			}

			if (false == postInteract(hitResult, interactionType))
			{
				TA_ASSERT_DEV(false, "PostInteract에 실패했습니다.");
				return;
			}
		}
	}
}

bool ATAPlayer::doInteract(FHitResult& hitResult, const ta::InteractionType& interactionType) noexcept
{
	if (false == hitResult.Actor.IsValid())
	{
		TA_ASSERT_DEV(false, "코드상 있을 수 없는 상황");
		return false;
	}

	switch (interactionType)
	{
	case ta::InteractionType::Attack:
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
	case ta::InteractionType::Greet:
		{
			// 추가해야한다.
		}
		break;
	case ta::InteractionType::Gather:
		{
			// 추가해야한다.
		}
		break;
	case ta::InteractionType::Talk:
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
		TA_COMPILE_DEV(4 == static_cast<ta::uint8>(ta::InteractionType::Count), "여기도 확인해주세요");
	}

	return true;
}

bool ATAPlayer::postInteract(FHitResult& hitResult, const ta::InteractionType& interactionType) noexcept
{
	if (false == hitResult.Actor.IsValid())
	{
		TA_ASSERT_DEV(false, "코드상 있을 수 없는 상황");
		return false;
	}

	switch (interactionType)
	{
	case ta::InteractionType::Attack:
		{
			// 추가해야한다.
		}
		break;
	case ta::InteractionType::Greet:
		{
			// 추가해야한다.
		}
		break;
	case ta::InteractionType::Gather:
		{
			// 추가해야한다.
		}
		break;
	case ta::InteractionType::Talk:
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
		TA_COMPILE_DEV(4 == static_cast<ta::uint8>(ta::InteractionType::Count), "여기도 확인해주세요");
	}

	return true;
}

void ATAPlayer::onViewChangedComplete(void) noexcept
{
	switch (_currentControlMode)
	{
	case ControlMode::PlayerThirdPerson:
		{
			// 보간이 완료되었으니 카메라가 이제 컨트롤로테이션이랑 같다.
			// 이제 맘편히 따라가도록 설정해주자.
			_springArm->bUsePawnControlRotation = true; // 이플래그 사용시 자체회전 x Pawn's ControlRotation을 받아옴
		}
		break;
	case ControlMode::FocusedAndControlBlocked:
		{
		}
		break;
	default:
		break;
	}
}

bool ATAPlayer::setControlMode(const ControlMode controlMode) noexcept
{
	// PlayerThirdPerson : 카메라 컨트롤로테이션 상속, 액터 로테이션 상속 안받음
	// PlayerFixedThirdPerson : 카메라 로테이션 고정, 액터 로테이션 상속 안받음
	// FocusedAndControlBlocked : 카메라 액터 로테이션 상속

	if (false == preSetControlMode(controlMode))
	{
		TA_ASSERT_DEV(false, "비정상입니다.");
		return false;
	}
	// UCharacterMovementComponent : 캐릭터 움직임 관리
	UCharacterMovementComponent* characterMovement = GetCharacterMovement();

	switch (controlMode)
	{
	case ControlMode::PlayerThirdPerson: 
		{
			// 액터로테이션 + 상대로테이션(원하는값) = 컨트롤 로테이션
			_toArmRelativeRotation = GetController()->GetControlRotation() - GetActorRotation();
			_toArmRelativePosition = FVector::ZeroVector;
			_toArmLength = _thirdPersonToArmLength;//450.0f;
			// 폰은 안따라가는데 컨트롤러 로테이션 카메라만 따라간다. 폰이 컨트롤러 로테이션 기준으로 움직이면 거기에 맞게 로테이션 

			// 상대위치 기준으로 보간이 완료되면 켜준다.
			// _springArm->bUsePawnControlRotation = true; // 이플래그 사용시 자체회전 x Pawn's ControlRotation을 받아옴
			
			_springArm->bInheritPitch = true; // 해당 회전값을 부모로 부터 받을지 결정 이미 bUsePawnControlRotation 사용중이니까 필요없다
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
				// 어차피 input에 따라서 움직이도록
				//characterMovement->bUseControllerDesiredRotation = false; // If true, smoothly rotate the Character toward the Controller's desired rotation 
				characterMovement->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
			}
		}
		break;
	case ControlMode::PlayerFixedThirdPerson:
		{
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
				characterMovement->bOrientRotationToMovement = false;
				characterMovement->bUseControllerDesiredRotation = true; // If true, smoothly rotate the Character toward the Controller's desired rotation 
				characterMovement->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
			}

			_toArmLength = _fixedThirdPersonToArmLength;
			_toArmRelativePosition = FVector::ZeroVector;
			_toArmRelativeRotation = _fixedThirdPersonRotator; // 로테이션 상속을 안받으니 무조건 Pitch -45로 고정이다.
		}
		break;
	case ControlMode::FocusedAndControlBlocked:
		{
			_springArm->bUsePawnControlRotation = false; // Pawn's ControlRotation을 받지 않음
			
			_springArm->bInheritPitch = true; // 상대로테이션 값으로 처리해야하기 때문에 true로 해준다.
			_springArm->bInheritRoll = true; // 상대로테이션 값으로 처리해야하기 때문에 true로 해준다.
			_springArm->bInheritYaw = true; // 상대로테이션 값으로 처리해야하기 때문에 true로 해준다.
			_springArm->bDoCollisionTest = true;
			bUseControllerRotationYaw = false; // bUseControllerDesiredRotation이걸로 보간해줘서 false로 해야한다. true로하면 끊긴다.


			const FVector targetActorPosition = _focusedCharacter->GetActorLocation();
			//{
			//	const float noise = 20.0f;
			//	FVector finalPositionInWorld = targetActorPosition + _focusedCharacter->GetActorRightVector() * noise; // 오른쪽으로 좀더
			//	finalPositionInWorld.Z += 50.0f; // 좀더 위쪽을 봐야하나?
			//}
			FVector finalPositionInWorld = targetActorPosition;

			// 스프링 암까지 포함한 카메라 최종위치에서 타겟을 바라보는 방향 벡터를 구한다.
			FVector lookAtVectorFromFinalPositionToTarget = targetActorPosition - (finalPositionInWorld + _focusedCharacter->GetActorForwardVector() * _focusedAndControlBlockedToArmLength);
			lookAtVectorFromFinalPositionToTarget.Normalize();

			FRotator finalRotationInWorld = lookAtVectorFromFinalPositionToTarget.Rotation();
			TA_LOG_DEV("FocusedAndControlBlocked Mode => finalRotationInWorld : %.1f, %.1f, %.1f"
					   , finalRotationInWorld.Roll
					   , finalRotationInWorld.Pitch
					   , finalRotationInWorld.Yaw);


			// 현재 arm은 PlayerActor좌표계에 있기 때문에 모두 상대적으로 계산해야한다.
			FRotator relativeStringArmRotation = finalRotationInWorld - GetActorRotation(); // 부모 액터 로테이션 + 컴포넌트 로테이션 => 최종 월드 로테이션
			FVector relativeStringArmPosition = GetActorLocation() - finalPositionInWorld;


			_toArmLength = _focusedAndControlBlockedToArmLength;
			_toArmRelativePosition = relativeStringArmPosition;
			_toArmRelativeRotation = relativeStringArmRotation;


			TA_LOG_DEV("FocusedAndControlBlocked Mode => focused actor<%d> => to position : %.1f, %.1f, %.1f / to rotation : %.1f, %.1f, %.1f"
					   , _focusedCharacter->getActorKey().getKeyValue()
					   , _toArmRelativePosition.X
					   , _toArmRelativePosition.Y
					   , _toArmRelativePosition.Z
					   , _toArmRelativeRotation.Roll
					   , _toArmRelativeRotation.Pitch
					   , _toArmRelativeRotation.Yaw);
		}
		break;
	default:
		{
			TA_ASSERT_DEV(false, "정의되지 않은 타입입니다.");
		}
		break;
	}

	if (false == postSetControlMode(controlMode))
	{
		TA_ASSERT_DEV(false, "비정상입니다.");
		return false;
	}

	_currentControlMode = controlMode;
	_cameraDirtyFlag = true;

	return true;
}


bool ATAPlayer::preSetControlMode(const ControlMode controlMode) noexcept
{
	switch (controlMode)
	{
	case ControlMode::PlayerThirdPerson:
		{
		}
		break;
	case ControlMode::PlayerFixedThirdPerson:
		{
		}
		break;
	case ControlMode::FocusedAndControlBlocked:
		{
			if (false == _focusedCharacter.IsValid())
			{
				TA_ASSERT_DEV(false, "비정상입니다.");
				return false;
			}
		}
		break;
	default:
		break;
	}

	return true;
}


bool ATAPlayer::postSetControlMode(const ControlMode controlMode) noexcept
{
	// PlayerThirdPerson : 카메라 컨트롤로테이션 상속, 액터 로테이션 상속 안받음
	// PlayerFixedThirdPerson : 카메라 로테이션 고정, 액터 로테이션 상속 안받음
	// FocusedAndControlBlocked : 카메라 액터 로테이션 상속 => 상대값으로 변경
	switch (_currentControlMode)
	{
	case ControlMode::PlayerThirdPerson:
		{
			switch (controlMode)
			{
			case ControlMode::FocusedAndControlBlocked:
				{
					// <SpringArm> : 폰의 컨트롤로테이션 => 액터의 상대로테이션으로 변경
					// GetActorRotation : 액터의 루트 컴포넌트의 로테이션 

					// 액터로테이션 + 상대로테이션(원하는 값) = 컨트롤 로테이션이 되어야 한다.
					_springArm->SetRelativeRotation(GetController()->GetControlRotation() - GetActorRotation());
					// 원래 pawn's control rotation을 사용하고 있었다 > 하지만 이제 사용안할것이기 때문에 현재의 절대값의 컨트롤 로테이션을 받아서 보간을 준비한다.
					// 만약 여기서 이처리를 해주지 않으면 상대값으로 사용하고 있던 카메라 로테이션값이 절대값으로 해석되면서 카메라가 튀게된다. 

					// 컨트롤 로테이션은 현재 액터가 바라보고 있는 방향으로 설정해준다.
					GetController()->SetControlRotation(GetActorRotation()); // PlayerThirdPerson => PlayerFixedThirdPerson // 이제 컨트롤 로테이션값은 화면의 의미가 없어지고 오직 액터의 방향만 나타낸다 // 액터기준으로 한번 세팅해준다.
				}
				break;
			case ControlMode::PlayerFixedThirdPerson:
				{
				}
				break;
			default:
				break;
			}
		}
		break;
	case ControlMode::PlayerFixedThirdPerson:
		{
			switch (controlMode)
			{
			case ControlMode::PlayerThirdPerson:
				{
					// <SpringArm> : 폰의 컨트롤로테이션 => 액터의 상대로테이션으로 변경
					// GetActorRotation : 액터의 루트 컴포넌트의 로테이션 

					// 액터로테이션 + 상대로테이션(원하는 값) = 컨트롤 로테이션이 되어야 한다.
					_springArm->SetRelativeRotation(GetController()->GetControlRotation() - GetActorRotation());
					// 원래 pawn's control rotation을 사용하고 있었다 > 하지만 이제 사용안할것이기 때문에 현재의 절대값의 컨트롤 로테이션을 받아서 보간을 준비한다.
					// 만약 여기서 이처리를 해주지 않으면 상대값으로 사용하고 있던 카메라 로테이션값이 절대값으로 해석되면서 카메라가 튀게된다. 

					// 컨트롤 로테이션은 현재 액터가 바라보고 있는 방향으로 설정해준다.
					GetController()->SetControlRotation(GetActorRotation()); // PlayerThirdPerson => PlayerFixedThirdPerson // 이제 컨트롤 로테이션값은 화면의 의미가 없어지고 오직 액터의 방향만 나타낸다 // 액터기준으로 한번 세팅해준다.
				}
				break;
			case ControlMode::FocusedAndControlBlocked:
				{
				}
				break;
			default:
				break;
			}
		}
		break;
	case ControlMode::FocusedAndControlBlocked:
		{
			switch (controlMode)
			{
			case ControlMode::PlayerThirdPerson:
				{
					// <SpringArm> : 폰의 컨트롤로테이션 => 액터의 상대로테이션으로 변경
					// GetActorRotation : 액터의 루트 컴포넌트의 로테이션 

					// 액터로테이션 + 상대로테이션(원하는 값) = 컨트롤 로테이션이 되어야 한다.
					_springArm->SetRelativeRotation(GetController()->GetControlRotation() - GetActorRotation());
					// 원래 pawn's control rotation을 사용하고 있었다 > 하지만 이제 사용안할것이기 때문에 현재의 절대값의 컨트롤 로테이션을 받아서 보간을 준비한다.
					// 만약 여기서 이처리를 해주지 않으면 상대값으로 사용하고 있던 카메라 로테이션값이 절대값으로 해석되면서 카메라가 튀게된다. 

					// 컨트롤 로테이션은 현재 액터가 바라보고 있는 방향으로 설정해준다.
					GetController()->SetControlRotation(GetActorRotation()); // PlayerThirdPerson => PlayerFixedThirdPerson // 이제 컨트롤 로테이션값은 화면의 의미가 없어지고 오직 액터의 방향만 나타낸다 // 액터기준으로 한번 세팅해준다.
				}
				break;
			case ControlMode::PlayerFixedThirdPerson:
				{
				}
				break;
			default:
				break;
			}

			// 현재 카메라가 보는 시점기준으로 한번세팅한다 // 이제 컨트롤러에 의해 카메라가 통제되기 때문에 싱크를 맞추기 위해서 카메라위치에 컨트롤러를 배치한다

			if (ControlMode::PlayerThirdPerson == controlMode)
			{
				// FocusedAndControlBlocked => PlayerThirdPerson
				// 아무것도 해주지않는다.
			}

			// 액터로테이션 + 상대로테이션(원하는 값) = 컨트롤 로테이션이 되어야 한다.
			//GetController()->SetControlRotation(GetActorRotation() + _springArm->GetRelativeRotation());
		}
		break;
	default:
		break;
	}

	return true;
}

void ATAPlayer::viewChange() noexcept
{
	if (ControlMode::PlayerThirdPerson == _currentControlMode)
	{
		setControlMode(ControlMode::FocusedAndControlBlocked);
	}
	else if (ControlMode::FocusedAndControlBlocked == _currentControlMode)
	{
		setControlMode(ControlMode::PlayerThirdPerson);
	}
	//switch (_currentControlMode)
	//{
	//case ControlMode::PlayerThirdPerson:
	//	{
	//		//_toArmRelativeRotation = GetActorRotation();
	//
	//		// GetActorRotation : 액터의 루트 컴포넌트의 로테이션 
	//		// PlayerThirdPerson모드에서 마우스를 움직일때 변경 : Controll , Pawn 로테이션(액터의 루트 컴포넌트의 로테이션)
	//		_springArm->SetRelativeRotation(GetController()->GetControlRotation());
	//		// 원래 pawn's control rotation을 사용하고 있었다 > 하지만 이제 사용안할것이기 때문에 현재의 절대값의 컨트롤 로테이션을 받아서 보간을 준비한다.
	//		// 만약 여기서 이처리를 해주지 않으면 상대값으로 사용하고 있던 카메라 로테이션값이 절대값으로 해석되면서 카메라가 튀게된다. 
	//		GetController()->SetControlRotation(GetActorRotation()); // PlayerThirdPerson => PlayerFixedThirdPerson // 이제 컨트롤 로테이션값은 화면의 의미가 없어지고 오직 액터의 방향만 나타낸다 // 액터기준으로 한번 세팅해준다.
	//		setControlMode(ControlMode::FocusedAndControlBlocked);
	//	}
	//	break;
	//case ControlMode::FocusedAndControlBlocked:
	//	{
	//		//_toArmRelativeRotation = _springArm->GetRelativeRotation();
	//		// 현재 카메라가 보는 시점기준으로 한번세팅한다 // 이제 컨트롤러에 의해 카메라가 통제되기 때문에 싱크를 맞추기 위해서 카메라위치에 컨트롤러를 배치한다
	//		GetController()->SetControlRotation(_springArm->GetRelativeRotation()); // PlayerFixedThirdPerson => PlayerThirdPerson
	//		setControlMode(ControlMode::PlayerThirdPerson);
	//	}
	//	break;
	//default:
	//	break;
	//}
}

void ATAPlayer::processSyncToServer(float deltaTime) noexcept
{
	_currentTimeForSync += deltaTime;
	if (_syncInterval < _currentTimeForSync)
	{
		_currentTimeForSync = 0.0f;
		if (false == getActorKey().isValid())
		{
			return;
		}

		ta::ClientActor* clientActor = getActorFromActorManager();
		if (nullptr == clientActor)
		{
			return;
		}

		ta::ClientMoveActorComponent* moveCom = ta::GetActorComponent<ta::ClientMoveActorComponent>(getActorKey());
		FVector currentPos;
		{
			ta::ScopedLock moveLock(moveCom, true);
			TAVectorToFVector(moveCom->getCurrentPosition_(), currentPos);
		}

		const FVector currentUEPos = GetActorLocation();
		if ((1.5f * 1.5f) > (GetActorLocation() - currentPos).SizeSquared())
		{
			return;
		}

		ta::Vector destination;
		FVectorToTAVector(currentUEPos, destination);
		ta::ClientMoveActorSystem* moveSystem = ta::GetActorSystem<ta::ClientMoveActorSystem>();
		if (false == moveSystem->requestMoveActor(clientActor, destination))
		{
			TA_ASSERT_DEV(false, "비정상입니다");
			return;
		}

		//TA_LOG_DEV("<SyncActor> => actorkey : %d, current position (%.1f, %.1f, %.1f)", getActorKey().getKeyValue(), destination._x, destination._y, destination._z);
	}
}

void ATAPlayer::toggleInventory() noexcept
{
	ATAPlayerController* controller = static_cast<ATAPlayerController*>(GetController());
	const bool toggleFlag = !(controller->getInventoryVisibility());
	controller->bShowMouseCursor = toggleFlag;
	controller->setInventoryVisibility(toggleFlag);
	TA_LOG_DEV("Toggle inventory : %d", toggleFlag);
}

void ATAPlayer::toggleMousePoint() noexcept
{
	ATAPlayerController* controller = static_cast<ATAPlayerController*>(GetController());
	controller->bShowMouseCursor = !(controller->bShowMouseCursor);
	TA_LOG_DEV("Toggle mouse point : %d", controller->bShowMouseCursor);
}

void ATAPlayer::key1Pressed() noexcept
{
	TA_LOG_DEV("keyF1Pressed");
	ATAPlayerController* controller = static_cast<ATAPlayerController*>(GetController());
	controller->setInteractionMenuVisibility(true);
}

void ATAPlayer::key2Pressed() noexcept
{
	TA_LOG_DEV("keyF2Pressed");
	ATAPlayerController* controller = static_cast<ATAPlayerController*>(GetController());
	controller->setInteractionMenuVisibility(false);
}

void ATAPlayer::key3Pressed() noexcept
{
	TA_LOG_DEV("key3Pressed");
}

void ATAPlayer::interaction1KeyPressed() noexcept
{
	TA_LOG_DEV("interaction1KeyPressed");
	if (false == processInteractionKeyPressed(InteractionKeyType::Interaction1Key))
	{
		TA_ASSERT_DEV(false, "비정상입니다");
	}
}

void ATAPlayer::interaction2KeyPressed() noexcept
{
	TA_LOG_DEV("interaction2KeyPressed");
	if (false == processInteractionKeyPressed(InteractionKeyType::Interaction2Key))
	{
		TA_ASSERT_DEV(false, "비정상입니다");
	}
}

void ATAPlayer::interaction3KeyPressed() noexcept
{
	TA_LOG_DEV("interaction3KeyPressed");
	if (false == processInteractionKeyPressed(InteractionKeyType::Interaction3Key))
	{
		TA_ASSERT_DEV(false, "비정상입니다");
	}
}

bool ATAPlayer::processInteractionKeyPressed(const InteractionKeyType& interactionKeyType) const noexcept
{
	if (InteractionKeyType::Count <= interactionKeyType)
	{
		TA_ASSERT_DEV(false, "비정상입니다");
		return false;
	}

	ATAPlayerController* controller = static_cast<ATAPlayerController*>(GetController());
	if (false == controller->getInteractionMenuVisibility())
	{
		TA_ASSERT_DEV(false, "비정상입니다");
		return false;
	}

	const uint8 buttonIndex = static_cast<uint8>(interactionKeyType);
	if (false == controller->executeInteractionButton(buttonIndex))
	{
		TA_ASSERT_DEV(false, "비정상입니다");
		return false;
	}

	return true;
}
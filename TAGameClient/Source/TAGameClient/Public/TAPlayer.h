// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TACharacter.h"
#include "Common/CommonDataTypeDefinition.h"
#include "TAPlayer.generated.h"


class USpringArmComponent;
class UCameraComponent;
class UTAInteractionButtonUserWidget;

/**
 * 
 */
UCLASS()
class TAGAMECLIENT_API ATAPlayer : public ATACharacter
{
	GENERATED_BODY()
	
public:
	ATAPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override final;



public:
	// Called every frame
	virtual void Tick(float DeltaTime) override final;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override final;
	virtual void PostInitializeComponents() override final;

	/* move, action, camera */
private:
	enum class CharacterBehaviorByInput : ta::uint8
	{
		Move = 0
		, Rotate
	};

	enum class DirectionType : ta::uint8
	{
		X = 0 // roll
		, Y	 // pitch
		, Z	 // yaw
	};

	enum class ControlMode : ta::uint8
	{
		PlayerThirdPerson = 0
		, PlayerFixedThirdPerson
		, FocusedAndControlBlocked
		, Count
	};


	
	void moveAndRotateCharacterByInput(const CharacterBehaviorByInput& characterBehaviorByInput, const DirectionType& directionType, const float axisValue) noexcept;
	void moveCharacterByInput(const DirectionType& directionType, const float axisValue) noexcept;
	void rotateCharacterByInput(const DirectionType& directionType, const float axisValue) noexcept;

	UFUNCTION()
	void upDown(const float axisValue) noexcept;

	UFUNCTION()
	void leftRight(const float axisValue) noexcept;

	UFUNCTION()
	void lookUp(const float axisValue) noexcept;

	UFUNCTION()
	void turn(const float axisValue) noexcept;

	UFUNCTION()
	void attack() noexcept;

	UFUNCTION()
	void onAttackMontageEnded(UAnimMontage* montage, bool interrupted) noexcept;

	void attackStartComboSate(void) noexcept;
	void attackEndComboSate(void) noexcept;

	UFUNCTION()
	void attackCheck() noexcept;

	UFUNCTION()
	void nextAttackCheck() noexcept;

	void interact(const ta::InteractionType& interactionType, float range, float radius) noexcept;
	bool doInteract(FHitResult& hitResult, const ta::InteractionType& interactionType) noexcept;
	bool postInteract(FHitResult& hitResult, const ta::InteractionType& interactionType) noexcept;

	void onViewChangedComplete(void) noexcept;
	
	bool setControlMode(const ControlMode controlMode) noexcept; // 플레그 세팅 등
	bool preSetControlMode(const ControlMode controlMode) noexcept; // 시도할 수 있는지 판단하는 검증 등
	bool postSetControlMode(const ControlMode controlMode) noexcept; // 보간시작전에 세팅해줘야할것들 등

	UFUNCTION()
	void viewChange() noexcept;

	void processSyncToServer(float deltaTime) noexcept;

private:

	UPROPERTY(VisibleAnyWhere, Category = Camera)
	USpringArmComponent* _springArm;

	UPROPERTY(VisibleAnyWhere, Category = Camera)
	UCameraComponent* _camera;


	ControlMode		_currentControlMode;
	FVector			_toMoveDirectionByInput;
	bool			_moveAndRotationDirtyFlag;

	// 카메라 전환시 보간 관련 변수
	bool			_cameraDirtyFlag;
	float			_toArmLength;
	FRotator		_toArmRelativeRotation; // ATAPlayer에 붙어있기 때문에 상대적
	FVector			_toArmRelativePosition; // ATAPlayer에 붙어있기 때문에 상대적
	float			_armLengthSpeed;
	float			_armRotationSpeed;
	float			_armPositionSpeed;



	// FocusedAndControlBlocked 모드를 위한 포커스 액터
	UPROPERTY()
	TWeakObjectPtr<ATACharacter> _focusedCharacter;

	UPROPERTY(EditAnyWhere, Category = Camera)
	float			_thirdPersonToArmLength;

	UPROPERTY(EditAnyWhere, Category = Camera)
	float			_fixedThirdPersonToArmLength;

	UPROPERTY(EditAnyWhere, Category = Camera)
	FRotator		_fixedThirdPersonRotator;

	UPROPERTY(EditAnyWhere, Category = Camera)
	float			_focusedAndControlBlockedToArmLength;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool _isAttacking;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool _canNextCombo;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool _isComboInputOn;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 _currentCombo;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 _maxCombo;

	// 클라이언트에서 서버로 동기화 시키기
	float _syncInterval;
	float _currentTimeForSync;

private:

	/* ui */
	enum class InteractionKeyType : ta::uint8
	{
		Interaction1Key = 0
		, Interaction2Key
		, Interaction3Key
		, Count
	};


	UFUNCTION()
	void toggleInventory() noexcept;

	UFUNCTION()
	void toggleMousePoint() noexcept;

	// for test
	UFUNCTION()
	void key1Pressed() noexcept;

	UFUNCTION()
	void key2Pressed() noexcept;

	UFUNCTION()
	void key3Pressed() noexcept;

	// for interaction
	UFUNCTION()
	void interaction1KeyPressed() noexcept;

	UFUNCTION()
	void interaction2KeyPressed() noexcept;

	UFUNCTION()
	void interaction3KeyPressed() noexcept;

	bool processInteractionKeyPressed(const InteractionKeyType& interactionKeyType) const noexcept;


private:

	//UPROPERTY()
	//TWeakObjectPtr<UTAInteractionButtonUserWidget> _interaction1KeyButton;
	//
	//UPROPERTY()
	//TWeakObjectPtr<UTAInteractionButtonUserWidget> _interaction2KeyButton;
	//
	//UPROPERTY()
	//TWeakObjectPtr<UTAInteractionButtonUserWidget> _interaction3KeyButton;
};

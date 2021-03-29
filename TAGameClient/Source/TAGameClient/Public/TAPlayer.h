// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TACharacter.h"
#include "TAPlayer.generated.h"


class USpringArmComponent;
class UCameraComponent;

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
		, Count
	};

	enum class InteractType : ta::uint8
	{
		Attack = 0
		, Gather
		, Count
	};
	
	// move
	void moveAndRotateCharacterByInput(const CharacterBehaviorByInput& characterBehaviorByInput, const DirectionType& directionType, const float axisValue) noexcept;
	void moveCharacterByInput(const DirectionType& directionType, const float axisValue) noexcept;
	void rotateCharacterByInput(const DirectionType& directionType, const float axisValue) noexcept;
	void upDown(const float axisValue) noexcept;
	void leftRight(const float axisValue) noexcept;
	void lookUp(const float axisValue) noexcept;
	void turn(const float axisValue) noexcept;

	// battle
	void attack(void) noexcept;
	UFUNCTION()
	void onAttackMontageEnded(UAnimMontage* montage, bool interrupted) noexcept;
	void attackStartComboSate(void) noexcept;
	void attackEndComboSate(void) noexcept;
	void attackCheck(void) noexcept;

	// interaction
	void interact(const InteractType& interactType, float range, float radius) noexcept;
	bool doInteract(FHitResult& hitResult, const InteractType& interactType) noexcept;
	bool postInteract(FHitResult& hitResult, const InteractType& interactType) noexcept;

	void setControlMode(const ControlMode controlMode) noexcept;
	void viewChange(void) noexcept;

	// ui
	void toggleInventory(void) noexcept;
	void toggleMousePoint(void) noexcept;

	// nav
	void exportRecastNavMesh(void) noexcept;

	void processSyncToServer(float deltaTime) noexcept;

public:
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
	FRotator		_toArmRotation;
	float			_armLengthSpeed;
	float			_armRotationSpeed;

	UPROPERTY(EditAnyWhere, Category = Camera)
	float			_thirdPersonToArmLength;

	UPROPERTY(EditAnyWhere, Category = Camera)
	float			_fixedThirdPersonToArmLength;



	//UPROPERTY(VisibleInstanceOnly)
//TWeakObjectPtr<ATAPawn> _target;
	/*
	TWeakObjectPtr : UObjects
	TWeakPtr : everything else
	*/

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
	float _maxTimeToSync;
	float _currentTimeToSync;
};

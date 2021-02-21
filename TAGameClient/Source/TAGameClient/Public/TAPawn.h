// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Common/CommonBase.h"
#include "TAPawn.generated.h"


class UCapsuleComponent;
class USkeletalMeshComponent;
class UFloatingPawnMovement;
class USpringArmComponent;
class UCameraComponent;
class UPawnMovementComponent;


UCLASS()
class TAGAMECLIENT_API ATAPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATAPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PostInitializeComponents(void) override final;

	virtual void PossessedBy(AController* NewController) override final;

private:
	void upDown(float newAxisValue) noexcept;
	void leftRight(float newAxisValue) noexcept;


private:
	UPROPERTY(VisibleAnyWhere, Category = Collision)
	UCapsuleComponent* _capsule;

	UPROPERTY(VisibleAnyWhere, Category = Visual)
	USkeletalMeshComponent* _mesh;

	UPROPERTY(VisibleAnyWhere, Category = Movement)
	UFloatingPawnMovement* _movement;

	UPROPERTY(VisibleAnyWhere, Category = Camera)
	USpringArmComponent* _springArm;

	UPROPERTY(VisibleAnyWhere, Category = Camera)
	UCameraComponent* _camera;

	// 캐싱
	UPROPERTY(VisibleAnyWhere)
	UPawnMovementComponent* _movementComponent;

};

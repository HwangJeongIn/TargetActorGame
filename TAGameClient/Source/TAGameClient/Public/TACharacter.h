// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Common/CommonBase.h"
#include "GameFramework/Character.h"
#include "TAPawn.h"
#include "TACharacter.generated.h"


class UTAAnimInstance;


UCLASS()
class TAGAMECLIENT_API ATACharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATACharacter();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PostInitializeComponents() override;

	//virtual bool movePosition(const FVector& newPosition, bool teleport = false) noexcept;
	//bool teleportToPosition(const FVector& newPosition) noexcept;
private:

	void setDeadAnimation(void) noexcept;


public:

	//UPROPERTY(VisibleInstanceOnly)
	//TWeakObjectPtr<ATAPawn> _target;
	/*
	TWeakObjectPtr : UObjects
	TWeakPtr : everything else
	*/

	UPROPERTY(VisibleInstanceOnly)
	UTAAnimInstance* _animInstance;

};

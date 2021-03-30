// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Common/CommonBase.h"
#include "UObject/ObjectMacros.h"
#include "Animation/AnimInstance.h"
#include "TAAnimInstance.generated.h"


//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnButtonClickedEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNextAttackCheckDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackHitCheckDelegate);

//DECLARE_MULTICAST_DELEGATE(FOnNextAttackCheckDelegate);
//DECLARE_MULTICAST_DELEGATE(FOnAttackHitCheckDelegate);

/**
 * 
 */
UCLASS()
class TAGAMECLIENT_API UTAAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UTAAnimInstance();
	
	virtual void NativeUpdateAnimation(float DeltaSeconds) override final;

	void playAttackMontage(void);
	void jumpToAttackMontageSection(const int32 newSection);
	void setDead(bool flag) noexcept;

	UPROPERTY(BlueprintAssignable)
	FOnNextAttackCheckDelegate	_onNextAttackCheck;

	UPROPERTY(BlueprintAssignable)
	FOnAttackHitCheckDelegate	_onAttackHitCheck;

private:
	UFUNCTION()
	void AnimNotify_AttackHitCheck();

	UFUNCTION()
	void AnimNotify_NextAttackCheck();

	FName getAttackMontageSectionName(const int32 section) noexcept;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	float _currentCharacterSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	float _maxWalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool _isInAir;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool _isDead;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	UAnimMontage* _attackMontage;
};

// Fill out your copyright notice in the Description page of Project Settings.


#include "TAAnimInstance.h"

UTAAnimInstance::UTAAnimInstance()
{
	_maxWalkSpeed = 300.0f;
	_maxSlowWalkSpeed = 200.0f;
	_currentCharacterSpeed = 0.0f;
	_isInAir = false;
	_isDead = false;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> ATTACK_MONTAGE(TEXT("/Game/_Dev/_Characters/Animations/SK_Mannequin_Skeleton_Montage.SK_Mannequin_Skeleton_Montage"));
	if (true == ATTACK_MONTAGE.Succeeded())
	{
		_attackMontage = ATTACK_MONTAGE.Object;
	}
}

void UTAAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (true == _isDead)
	{
		return;
	}

	APawn* pawn = TryGetPawnOwner();
	if (false == ::IsValid(pawn))
	{
		return;
	}

	{
		_currentCharacterSpeed = pawn->GetVelocity().Size();

		ACharacter* character = Cast<ACharacter>(pawn);
		if (nullptr != character)
		{
			_isInAir = character->GetCharacterMovement()->IsFalling();
		}

	}
}

void UTAAnimInstance::playAttackMontage(void)
{
	if (true == _isDead)
	{
		return;
	}

	//if (false == Montage_IsPlaying(_attackMontage))
	{
		//TA_ASSERT_DEV(false == _isDead, "죽은 상태인데 몽타주를 수행하려합니다. 왜그런지 확인해야합니다.");
		Montage_Play(_attackMontage, 1.0f);
	}
}

void UTAAnimInstance::jumpToAttackMontageSection(const int32 newSection)
{
	if (true == _isDead)
	{
		return;
	}

	TA_LOG_DEV("new section : %d", newSection);
	TA_LOG_DEV("is playing attack montage : %d", Montage_IsPlaying(_attackMontage));
	TA_LOG_DEV("current section montage name : %s", *Montage_GetCurrentSection(_attackMontage).ToString());

	//TA_ASSERT_DEV(false == _isDead, "죽은 상태인데 몽타주를 수행하려합니다. 왜그런지 확인해야합니다.");
	Montage_JumpToSection(getAttackMontageSectionName(newSection), _attackMontage);
}

void UTAAnimInstance::setDead(bool flag) noexcept
{
	_isDead = flag;
}

void UTAAnimInstance::AnimNotify_AttackHitCheck()
{
	_onAttackHitCheck.Broadcast();
}

void UTAAnimInstance::AnimNotify_NextAttackCheck()
{
	_onNextAttackCheck.Broadcast();
}

FName UTAAnimInstance::getAttackMontageSectionName(const int32 section) noexcept
{
	if (false == FMath::IsWithinInclusive<int32>(section, 1, 4))
	{
		TA_ASSERT_DEV(false, "비정상적인 인덱스 입니다. %d", section);
		return FName();
	}

	return FName(*FString::Printf(TEXT("Attack%d"), section));
}
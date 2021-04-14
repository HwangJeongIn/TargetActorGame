// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Common/CommonBase.h"
#include "Common/KeyDefinition.h"
#include "GameFramework/Character.h"
#include "TAActor.h"
#include "TACharacter.generated.h"


class UTAAnimInstance;
struct FStreamableHandle;

namespace ta
{
	class ClientActor;
}


UCLASS()
class TAGAMECLIENT_API ATACharacter : public ACharacter, public TAActor
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATACharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void BeginPlay() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PostInitializeComponents() override;


	bool setSkeletalMeshAndAnimInstance(const FString& skeletalMeshPath, const FString& animInstancePath/*, const bool reserveExecutionOnPlay*/) noexcept;
	void setCharacterHeight(const float height) noexcept;
	float getCharacterHalfHeight(void) const noexcept;

	UFUNCTION()
	void onSkeletalMeshAssetLoadCompleted() noexcept;

	UFUNCTION()
	void onAnimInstanceAssetLoadCompleted() noexcept;

private:
	void setDeadAnimation(void) noexcept;


public:
	/*
	TWeakObjectPtr : UObjects
	TWeakPtr : everything else
	*/

	UPROPERTY(VisibleInstanceOnly)
	UTAAnimInstance* _animInstance;

private:

	UPROPERTY()
	float _characterHalfHeight;

	UPROPERTY()
	FString _skeletalMeshPath;

	UPROPERTY()
	FString _animInstancePath;

	//UPROPERTY()
	TSharedPtr<FStreamableHandle> _skeletalMeshAssetStreamingHandle;

	//UPROPERTY()
	TSharedPtr<FStreamableHandle> _animInstanceAssetStreamingHandle;

};

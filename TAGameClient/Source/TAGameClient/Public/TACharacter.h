// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Common/CommonBase.h"
#include "Common/KeyDefinition.h"
#include "GameFramework/Character.h"
#include "TAPawn.h"
#include "TACharacter.generated.h"


class UTAAnimInstance;
struct FStreamableHandle;

namespace ta
{
	class ClientActor;
}


UCLASS()
class TAGAMECLIENT_API ATACharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATACharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PostInitializeComponents() override;


	bool setSkeletalMeshAndAnimInstance(const FString& skeletalMeshPath, const FString& animInstancePath) noexcept;
	
	void onSkeletalMeshAssetLoadCompleted(void) noexcept;
	void onAnimInstanceAssetLoadCompleted(void) noexcept;

	void resetActorKey(void) noexcept;
	bool setActorKey(const ta::ActorKey& actorKey) noexcept;
	const ta::ActorKey& getActorKey(void) const noexcept;
	ta::ClientActor* getActorFromActorManager(void) const noexcept;

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
	ta::ActorKey _actorKey;

	UPROPERTY()
	FString _skeletalMeshPath;

	UPROPERTY()
	FString _animInstancePath;

	//UPROPERTY()
	TSharedPtr<FStreamableHandle> _skeletalMeshAssetStreamingHandle;

	//UPROPERTY()
	TSharedPtr<FStreamableHandle> _animInstanceAssetStreamingHandle;

};

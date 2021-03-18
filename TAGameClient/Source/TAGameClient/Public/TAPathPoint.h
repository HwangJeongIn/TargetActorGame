// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TANotSpawnedActor.h"
#include "TAPathPoint.generated.h"


UCLASS()
class TAGAMECLIENT_API ATAPathPoint : public ATANotSpawnedActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATAPathPoint();

	ATAPathPoint* getNext(void) noexcept;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

private:
	void setPointColor(const bool isSinglePathPoint) noexcept;


public:
	UPROPERTY()
	bool _isValidMaterial;

	UPROPERTY(EditAnyWhere)
	UMaterialInstanceDynamic* _materialInstance;

	UPROPERTY(EditAnyWhere)
	TWeakObjectPtr<ATAPathPoint> _next;
};

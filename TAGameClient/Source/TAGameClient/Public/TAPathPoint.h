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


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

private:
	void setPointInfo(const bool isSinglePathPoint) noexcept;
	void setPointColor(const bool isSinglePathPoint) noexcept;

	ATAPathPoint* getNext(void) noexcept;
	ATAPathPoint* getPrev(void) noexcept;

	void setNext(ATAPathPoint* input) noexcept;
	void setPrev(ATAPathPoint* input) noexcept;


public:
	UPROPERTY()
	bool _isValidMaterial;

	UPROPERTY()
	float _defaultHeight;

	UPROPERTY()
	float _defaultHeightScale;

	UPROPERTY(EditAnyWhere)
	UMaterialInstanceDynamic* _materialInstance;

	UPROPERTY(EditAnyWhere)
	TWeakObjectPtr<ATAPathPoint> _next;

	UPROPERTY(EditAnyWhere)
	TWeakObjectPtr<ATAPathPoint> _prev;

	UPROPERTY(EditAnyWhere)
	FVector _destination;

	UPROPERTY(VisibleAnyWhere)
	UStaticMesh* _sphere; 

	UPROPERTY(VisibleAnyWhere)
	UStaticMesh* _cone;
};

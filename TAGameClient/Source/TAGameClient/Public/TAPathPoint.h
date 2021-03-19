// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TANotSpawnedActor.h"
#include "TAPathPoint.generated.h"


int32 GetTargetFolderPathPoints(ULevel* level, const FString& targetFolderName, TArray<ATAPathPoint*>& output) noexcept;

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
	void setDefaultPathPoint(void) noexcept;
	void refreshPathPoint(void) noexcept;
	void setPointColor(const bool isSinglePathPoint) noexcept;

	ATAPathPoint* getNext(void) noexcept;
	ATAPathPoint* getPrev(void) noexcept;

	void setNext(ATAPathPoint* input) noexcept;
	void setPrev(ATAPathPoint* input) noexcept;

public:
	UPROPERTY(EditAnyWhere, Category = ATAPathPoint)
	bool _refreshCurrentPath;

	UPROPERTY()
	bool _isValidMaterial;

	UPROPERTY()
	float _defaultHeight;

	UPROPERTY()
	float _defaultHeightScale;

	UPROPERTY()
	float _defaultWidthScale;

	UPROPERTY(EditAnyWhere, Category = ATAPathPoint)
	TWeakObjectPtr<ATAPathPoint> _next;

	UPROPERTY(EditAnyWhere, Category = ATAPathPoint)
	TWeakObjectPtr<ATAPathPoint> _prev;

	UPROPERTY(EditAnyWhere, Category = ATAPathPoint)
	UMaterialInstanceDynamic* _materialInstance;

	UPROPERTY(VisibleAnyWhere, Category = ATAPathPoint)
	UStaticMesh* _sphere; 

	UPROPERTY(VisibleAnyWhere, Category = ATAPathPoint)
	UStaticMesh* _cone;
};

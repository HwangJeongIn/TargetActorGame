// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TANotSpawnedActor.h"
#include "Common/KeyDefinition.h"
#include "TASpawnData.generated.h"


/**
 * 
 */
UCLASS()
class TAGAMECLIENT_API ATASpawnData : public ATANotSpawnedActor
{
	GENERATED_BODY()
public:
	ATASpawnData();

public:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override final;

private:
	//void setDefaultPathPoint(void) noexcept;
	//void refreshPathPoint(void) noexcept;
	//void setPointColor(const bool isSinglePathPoint) noexcept;
	//
	//ATAPathPoint* getNext(void) noexcept;
	//ATAPathPoint* getPrev(void) noexcept;
	//
	//void setNext(ATAPathPoint* input) noexcept;
	//void setPrev(ATAPathPoint* input) noexcept;

public:

	UPROPERTY(EditAnyWhere, Category = ATASpawnData)
	bool _attachToTheGround;

	// typedef로 키의 원시자료형에 맞추려고 했는데 typedef를 클래스로 인식해버린다.
	UPROPERTY(EditAnyWhere, Category = ATASpawnData)
	int _groupGameDataKey;
};

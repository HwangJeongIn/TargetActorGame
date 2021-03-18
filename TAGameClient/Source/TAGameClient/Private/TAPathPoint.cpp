// Fill out your copyright notice in the Description page of Project Settings.


#include "TAPathPoint.h"
#include "Components/StaticMeshComponent.h"
#include "Common/CommonBase.h"

// Sets default values
ATAPathPoint::ATAPathPoint()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	_staticMesh->SetStaticMesh(SphereMeshAsset.Object);
	_isValidMaterial = false;
	_materialInstance = nullptr;
}

// Called when the game starts or when spawned
void ATAPathPoint::BeginPlay()
{
	Super::BeginPlay();

	setPointColor(true);
}

// Called every frame
void ATAPathPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATAPathPoint::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(ATAPathPoint, _next))
	{
		TA_LOG_DEV("_next is changed");
		/* Because you are inside the class, you should see the value already changed */
		if (nullptr == getNext())
		{
			setPointColor(true);
		}
		else
		{
			setPointColor(false);
		}

		///* if you want to use bool property */
		//UWeakObjectProperty* prop = static_cast<UWeakObjectProperty*>(PropertyChangedEvent.Property);
		//if (prop->GetPropertyValue())
		//	dothings()
		//else
		//	undothings()
	}
}

void ATAPathPoint::setPointColor(const bool isSinglePathPoint) noexcept
{
	if (nullptr == _materialInstance)
	{
		if (true == _isValidMaterial)
		{
			return;
		}

		_isValidMaterial = true;
		//TArray<class UMaterialInterface*> materials = _staticMesh->GetMaterials();
		//if (0 == materials.Num())
		//{
		//	return;
		//}
		//_materialInstance = UMaterialInstanceDynamic::Create(materials[0], this);

		UMaterialInterface* material = _staticMesh->GetMaterial(0);
		if (nullptr == material)
		{
			TA_LOG_DEV("nullptr == material");
			return;
		}

		_materialInstance = _staticMesh->CreateDynamicMaterialInstance(0, material);
		if (nullptr == _materialInstance)
		{
			return;
		}

		TA_LOG_DEV("CreateDynamicMaterialInstance");
		//_materialInstance = _staticMesh->CreateAndSetMaterialInstanceDynamic(0);
	}

	if (true == isSinglePathPoint)
	{
		TA_LOG_DEV("SinglePathPointColor");
		_materialInstance->SetVectorParameterValue("SinglePathPointColor", FVector(0.0f, 1.0f, 0.0f));// FLinearColor(255.0f, 0.0f, 0.0f, 1.0f));
	}
	else
	{
		TA_LOG_DEV("MultiplePathPointColor");
		_materialInstance->SetVectorParameterValue("MultiplePathPointColor", FVector(1.0f, 0.0f, 0.0f));//FLinearColor(0.0f, 255.0f, 0.0f, 1.0f));
	}
	_staticMesh->SetMaterial(0, _materialInstance);
}

ATAPathPoint* ATAPathPoint::getNext(void) noexcept
{
	if (nullptr == _next.Get())
	{
		TA_LOG_DEV("next is nullptr");
		return nullptr;
	}

	if (false == _next.IsValid())
	{
		TA_LOG_DEV("next is invalid ptr");
		return nullptr;
	}

	return _next.Get();
}
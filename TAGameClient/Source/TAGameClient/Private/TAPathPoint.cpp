// Fill out your copyright notice in the Description page of Project Settings.


#include "TAPathPoint.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Common/CommonBase.h"

// Sets default values
ATAPathPoint::ATAPathPoint()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ConeMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cone.Cone'"));

	_sphere = SphereMeshAsset.Object;
	_cone = ConeMeshAsset.Object;
	

	_staticMesh->SetStaticMesh(_cone);
	_isValidMaterial = false;
	_materialInstance = nullptr;

	// SetActorScale3D 스케일 계산은 생성자에서 적용되지 않는다.
	// GetComponentsBoundingBox는 생성자에서 0, 0, 0이 나온다. // 아직 계산되기 전일듯
	// StaticMesh의 Bound의 경우 생성자에서 SetActorScale3D으로 값을 변경해도 초기값 그대로 나온다. 

	SetActorScale3D(FVector(0.5f, 0.5f, 1.0f));
	_defaultHeight = _staticMesh->Bounds.GetBox().GetSize().Z;
	_defaultHeightScale = _staticMesh->GetComponentScale().Z;
}

// Called when the game starts or when spawned
void ATAPathPoint::BeginPlay()
{
	Super::BeginPlay();

	setPointInfo(true);
	//setPointColor(true);
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
	TA_LOG_DEV("PropertyName : %s", *PropertyName.ToString());
	if (PropertyName == GET_MEMBER_NAME_CHECKED(ATAPathPoint, _next))
	{
		TA_LOG_DEV("_next is changed");
		/* Because you are inside the class, you should see the value already changed */
		if (nullptr == getNext())
		{
			setPointInfo(true);
		}
		else
		{
			setPointInfo(false);
		}

		///* if you want to use bool property */
		//UWeakObjectProperty* prop = static_cast<UWeakObjectProperty*>(PropertyChangedEvent.Property);
		//if (prop->GetPropertyValue())
		//	dothings()
		//else
		//	undothings()
	}
}

void ATAPathPoint::setPointInfo(const bool isSinglePathPoint) noexcept
{
	//SetActorScale3D(FVector(1.0f, 1.0f, 1.0f));
	//FVector temp = GetComponentsBoundingBox(true, false).GetSize();// _staticMesh->Bounds.GetBox().GetSize();
	//TA_LOG_DEV("%.1f, %.1f, %.1f", temp.X, temp.Y, temp.Z);
	//
	//SetActorScale3D(FVector(1.0f, 1.0f, 3.0f));
	//temp = GetComponentsBoundingBox(true, false).GetSize(); //_staticMesh->Bounds.GetBox().GetSize();
	//TA_LOG_DEV("%.1f, %.1f, %.1f", temp.X, temp.Y, temp.Z);
	//
	//SetActorScale3D(FVector(1.0f, 1.0f, 1.0f));
	//temp = GetComponentsBoundingBox(false, false).GetSize();// _staticMesh->Bounds.GetBox().GetSize();
	//TA_LOG_DEV("%.1f, %.1f, %.1f", temp.X, temp.Y, temp.Z);
	//
	//SetActorScale3D(FVector(1.0f, 1.0f, 3.0f));
	//temp = GetComponentsBoundingBox(false, false).GetSize(); //_staticMesh->Bounds.GetBox().GetSize();
	//TA_LOG_DEV("%.1f, %.1f, %.1f", temp.X, temp.Y, temp.Z);
	//
	//SetActorScale3D(FVector(1.0f, 1.0f, 1.0f));
	//temp = _staticMesh->Bounds.GetBox().GetSize();
	//TA_LOG_DEV("%.1f, %.1f, %.1f", temp.X, temp.Y, temp.Z);
	//
	//SetActorScale3D(FVector(1.0f, 1.0f, 3.0f));
	//temp = _staticMesh->Bounds.GetBox().GetSize();
	//TA_LOG_DEV("%.1f, %.1f, %.1f", temp.X, temp.Y, temp.Z);
	



	if (false == isSinglePathPoint)
	{
		_staticMesh->SetStaticMesh(_cone);

		ATAPathPoint* pathPoint = getNext();
		if (nullptr == pathPoint)
		{
			TA_ASSERT_DEV(false, "비정상");
			return;
		}

		if (this == pathPoint)
		{
			TA_ASSERT_DEV(false, "같은 포인트");
			return;
		}
		
		FVector origin = GetActorLocation();
		FVector destination = pathPoint->GetActorLocation();
		
		FVector directionWithScalar = pathPoint->GetActorLocation() - GetActorLocation();
		const float distance = directionWithScalar.Size();
		FVector directionOnly = directionWithScalar;
		directionOnly.Normalize();


		TA_ASSERT_DEV(0.0f != _defaultHeight, "비정상");
		TA_ASSERT_DEV(0.0f != _defaultHeightScale, "비정상");

		float finalScaleZ = (distance / _defaultHeight) / _defaultHeightScale;
		

		FVector finalScale = GetActorScale3D();
		finalScale.Z = finalScaleZ;
		SetActorScale3D(finalScale);

		// Actor의 시작점이 StaticMesh의 가장 아래로 세팅
		setActorLocationAsStaticMeshBottom(distance);
		
		//UWorld* world = GetWorld();
		//if (nullptr == world)
		//{
		//	TA_ASSERT_DEV(false, "비정상");
		//	return;
		//}
		////DrawDebugLine(world, GetActorLocation(), pathPoint->GetActorLocation(), FColor::Magenta, true, -1, 0, 2);

	}
	else
	{
		_staticMesh->SetStaticMesh(_sphere);
	}

	//setPointColor(isSinglePathPoint);
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

ATAPathPoint* ATAPathPoint::getPrev(void) noexcept
{
	if (nullptr == _prev.Get())
	{
		TA_LOG_DEV("prev is nullptr");
		return nullptr;
	}

	if (false == _prev.IsValid())
	{
		TA_LOG_DEV("prev is invalid ptr");
		return nullptr;
	}

	return _prev.Get();
}

void ATAPathPoint::setNext(ATAPathPoint* input) noexcept
{
	_next = input;
}

void ATAPathPoint::setPrev(ATAPathPoint* input) noexcept
{
	_prev = input;
}
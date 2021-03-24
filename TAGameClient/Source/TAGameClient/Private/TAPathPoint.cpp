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

	_staticMesh->SetStaticMesh(_sphere);

	_defaultHeight = _staticMesh->Bounds.GetBox().GetSize().Z;
	_defaultHeightScale = _staticMesh->GetComponentScale().Z;
	_defaultWidthScale = _staticMesh->GetComponentScale().X;

	_refreshCurrentPath = false;

	setDefaultPathPoint();
}

void ATAPathPoint::BeginPlay()
{
	Super::BeginPlay();

	//setPointColor(true);
}

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
		// next가 바뀌었을때 해당 next에 접근해서 prev도 바꿔주자
		TA_LOG_DEV("_next is changed");
		refreshPathPoint();

		///* if you want to use bool property */
		//UWeakObjectProperty* prop = static_cast<UWeakObjectProperty*>(PropertyChangedEvent.Property);
		//if (prop->GetPropertyValue())
		//	dothings()
		//else
		//	undothings()
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(ATAPathPoint, _refreshCurrentPath))
	{
		TA_LOG_DEV("_refreshCurrentPath is changed : %d", _refreshCurrentPath);

		if (true == _refreshCurrentPath)
		{
			//_refreshCurrentPath = false;
			ULevel* currentLevel = GetLevel();
			if (nullptr == currentLevel)
			{
				TA_ASSERT_DEV(false, "current level is nullptr");
				return;
			}

			if ("PathPointLevel" != currentLevel->GetOuter()->GetName())
			{
				TA_ASSERT_DEV(false, "current level is not PathPointLevel");
				return;
			}

			FString ownerFolderName;
			if (false == TAGetFolderName(GetFolderPath().ToString(), ownerFolderName, 1))
			{
				TA_ASSERT_DEV(false, "비정상");
				return;
			}


			TArray<ATAPathPoint*> folderPathPoints;
			const int32 folderPathPointCount = TAGetTargetLevelActorsInFolder<ATAPathPoint>(currentLevel, ownerFolderName, folderPathPoints);
			if (0 == folderPathPointCount)
			{
				TA_LOG_DEV("0 == folderPathPointCount");
				return;
			}

			ATAPathPoint* prevPathPoint = nullptr;
			for (ATAPathPoint* currentPathPoint : folderPathPoints)
			{ 
				if (nullptr != prevPathPoint)
				{
					prevPathPoint->setNext(currentPathPoint);
					prevPathPoint->refreshPathPoint();
					currentPathPoint->setPrev(prevPathPoint);
				}

				prevPathPoint = currentPathPoint;
			}
		}
	}
}

void ATAPathPoint::setDefaultPathPoint(void) noexcept
{
	_staticMesh->SetStaticMesh(_sphere);
	_staticMesh->SetRelativeScale3D(FVector(_defaultWidthScale / 2, _defaultWidthScale / 2, _defaultHeightScale / 2));
	SetActorRotation(FQuat::Identity);
}

void ATAPathPoint::refreshPathPoint(void) noexcept
{
	ATAPathPoint* next = getNext();
	if (nullptr == next)
	{
		setDefaultPathPoint();
		TA_LOG_DEV("single path point");
		return;
	}

	// nullptr != next
	if (this == next)
	{
		setDefaultPathPoint();
		TA_LOG_DEV("같은 포인트");
		return;
	}

	//next->setPrev(this);

	_staticMesh->SetStaticMesh(_cone);
	// extend cone to next path point and rotate actor
	{
		FVector origin = GetActorLocation();
		FVector destination = next->GetActorLocation();

		FVector directionWithScalar = destination - origin;
		const float distance = directionWithScalar.Size();
		FVector directionOnly = directionWithScalar;
		directionOnly.Normalize();

		TA_ASSERT_DEV(0.0f != _defaultHeight, "비정상");
		TA_ASSERT_DEV(0.0f != _defaultHeightScale, "비정상");

		float finalScaleZ = (distance / _defaultHeight) / _defaultHeightScale;

		// 부모 Component가 1이라서 사실상 Relative지만 World로 보는게 맞다.
		FVector finalScale = _staticMesh->GetRelativeScale3D();
		finalScale.Z = finalScaleZ;
		_staticMesh->SetRelativeScale3D(finalScale);

		// Actor의 시작점이 StaticMesh의 가장 아래로 세팅
		setActorLocationAsStaticMeshBottom(distance);

		SetActorRotation(FRotationMatrix::MakeFromZ(directionOnly).ToQuat());
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
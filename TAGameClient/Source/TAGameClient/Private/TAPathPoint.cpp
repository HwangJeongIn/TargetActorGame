// Fill out your copyright notice in the Description page of Project Settings.


#include "TAPathPoint.h"
#include "Components/SphereComponent.h"

// Sets default values
ATAPathPoint::ATAPathPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	_sphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SPHERE"));

	RootComponent = _sphereComponent;

}

// Called when the game starts or when spawned
void ATAPathPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATAPathPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


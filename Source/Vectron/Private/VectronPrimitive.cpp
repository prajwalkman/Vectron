// Fill out your copyright notice in the Description page of Project Settings.

#include "VectronPrivatePCH.h"
#include "VectronPrimitive.h"

// Sets default values
AVectronPrimitive::AVectronPrimitive()
{}

AVectronPrimitive::AVectronPrimitive(const class FObjectInitializer& PCIP) : Super(PCIP)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	primitiveMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Primitive VF Modifier"));

	SetupSMComponentWithCollision(primitiveMeshComponent);
	
	RootComponent = primitiveMeshComponent;

}

void AVectronPrimitive::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (!FVectronModule::Get().ActivePrimitives.Contains(this))
		FVectronModule::Get().ActivePrimitives.Add(this);

	FVectronModule::Get().ManualUpdateDelegate.ExecuteIfBound();
}

// Called when the game starts or when spawned
void AVectronPrimitive::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVectronPrimitive::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

bool AVectronPrimitive::isVoxelInPrimitive(FVector voxelPosition)
{
	FVector closestCollisionPosition;
	float result = primitiveMeshComponent->GetDistanceToCollision(voxelPosition, closestCollisionPosition);
	if (result == 0.0f)
	{
		return true;
	}
	else
	{
		return false;
	}
}

FVector AVectronPrimitive::fieldDirectionAtPosition(FVector voxelPosition)
{
	//DLOG("Actor Location: " + GetActorLocation().ToString());
	FVector dir;
	if (isVoxelInPrimitive(voxelPosition))
	{
		switch (primitiveType)
		{
		case EPrimitiveActorType::PAT_CONSTANT:
			dir = constantForce;
			break;
		case EPrimitiveActorType::PAT_ATTRACTIVE:
			dir = GetActorLocation() - voxelPosition;
			break;
		case EPrimitiveActorType::PAT_REPULSIVE:
			dir = voxelPosition - GetActorLocation();
			break;
		default:
			dir = FVector::ZeroVector;
		}
		dir.Normalize();
		return intensity * dir;
	}
	return FVector::ZeroVector;
}

AVectronPrimitive::~AVectronPrimitive()
{
	FVectronModule::Get().ActivePrimitives.Remove(this);
}

void AVectronPrimitive::PostEditMove(bool bFinished)
{
	FVectronModule::Get().ManualUpdateDelegate.ExecuteIfBound();
}
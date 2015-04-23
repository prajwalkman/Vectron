// Fill out your copyright notice in the Description page of Project Settings.

#include "VectronPrivatePCH.h"
#include "VectronBoundingBox.h"

// Sets default values
AVectronBoundingBox::AVectronBoundingBox()
{}

AVectronBoundingBox::AVectronBoundingBox(const class FObjectInitializer& PCIP) : Super(PCIP)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AVectronBoundingBox::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVectronBoundingBox::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AVectronBoundingBox::setFFGAContents(FFGAContents* importedValue)
{
	m_bbContents = importedValue;
}

FVector AVectronBoundingBox::getVoxelPosition(int32 x, int32 y, int32 z)
{
	FVector origin = GetActorLocation();
	FVector size = m_bbContents->Bounds.GetSize();
	FVector voxelPosition(
		size.X / m_bbContents->GridX / 2,
		size.Y / m_bbContents->GridY / 2,
		size.Z / m_bbContents->GridZ / 2
	);

	// Translate indexes from [0, Max) to [-Max/2, Max/2) because the origin for actors is at the center
	x -= m_bbContents->GridX / 2;
	y -= m_bbContents->GridY / 2;
	z -= m_bbContents->GridZ / 2;
	FVector indexes(x, y, z);

	// translate the calculated voxel position by origin of the actor to get the correct voxel position in the world
	return FVector(indexes * voxelPosition + origin);
}

void AVectronBoundingBox::PostActorCreated()
{
	Super::PostInitializeComponents();

	setFFGAContents(FVectronModule::Get().m_escrowFga);
	test = m_bbContents->GridX;
}

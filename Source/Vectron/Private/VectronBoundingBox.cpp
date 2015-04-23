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

	m_contentReference = FFGAContents();
}

// Called before Actor components are initialized
void AVectronBoundingBox::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	setFFGAContents(FVectronModule::Get().m_escrowFga);
	m_contentReference = *m_bbContents;
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

FVector AVectronBoundingBox::getResolvedIndex(int32 index)
{
	FVector resolvedIndex(0, 0, 0);

	int32 bitsX = (int)(floor(log2f(m_bbContents->GridX - 1)) + 1);
	int32 bitsY = (int)(floor(log2f(m_bbContents->GridY - 1)) + 1);
	int32 bitsZ = (int)(floor(log2f(m_bbContents->GridZ - 1)) + 1);
	//DLOG("X bits: %d, Y bits: %d, Z bits: %d", bitsX, bitsY, bitsZ);
	int32 shifter = ((1 << bitsX) - 1);
	resolvedIndex.X = shifter & index;
	shifter = (((1 << bitsY) - 1) << bitsX);
	resolvedIndex.Y = (shifter & index) >> bitsX;
	shifter = ((1 << bitsZ) - 1) << (bitsX + bitsY);
	resolvedIndex.Z = (shifter & index) >> (bitsX + bitsY);

	DLOG("Resolved Index: " + resolvedIndex.ToString());
	
	return resolvedIndex;
}
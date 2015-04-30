// Fill out your copyright notice in the Description page of Project Settings.

#include "VectronPrivatePCH.h"
#include "VectronBoundingBox.h"
#include "VectronPrimitive.h"

// Sets default values
//AVectronBoundingBox::AVectronBoundingBox()
//{}

AVectronBoundingBox::AVectronBoundingBox(const class FObjectInitializer& PCIP) : Super(PCIP)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	emptyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Empty SceneComponent"));

	RootComponent = emptyRoot;
}

void AVectronBoundingBox::OnConstruction(const FTransform& ft)
{
	Super::OnConstruction(ft);

	if (FVectronModule::Get().m_escrowFga == nullptr) return;
	setFFGAContents(FVectronModule::Get().m_escrowFga);
	RenderField();
}

// Called before Actor components are initialized
void AVectronBoundingBox::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	setFFGAContents(FVectronModule::Get().m_escrowFga);

	
}

void AVectronBoundingBox::PostInitializeComponents()
{

	Super::PostInitializeComponents();

}

// Called when the game starts or when spawned
void AVectronBoundingBox::BeginPlay()
{
	Super::BeginPlay();
}

void AVectronBoundingBox::RenderField()
{
	FlushPersistentDebugLines(GetWorld());

	for (size_t i = 0; i < m_bbContents->Vectors.Num(); i++) {
		m_bbContents->Vectors[i].Normalize();
		auto ri = getResolvedIndex(i);
		auto vpos = getVoxelPosition(ri.X, ri.Y, ri.Z);
		if (m_bbContents->Vectors[i].IsNearlyZero(0.001f))
		{
			DrawDebugPoint(GetWorld(), vpos, 3.0f, FColor::Black, true);
			continue;
		}
		FVector dir = m_bbContents->Vectors[i];
		for (auto p : FVectronModule::Get().ActivePrimitives)
		{
			dir += p->fieldDirectionAtPosition(vpos);
		}
		auto vrayend = vpos + dir * intensity;
		DrawDebugDirectionalArrow(GetWorld(), vpos, vrayend, 3.0f, FColor::Black, true);
	}
	DrawDebugBox(GetWorld(), GetActorLocation(), m_bbContents->Bounds.GetExtent(), FColor::Blue, true);
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

FFGAContents* AVectronBoundingBox::getFFGAContents()
{
	return m_bbContents;
}
FVector AVectronBoundingBox::getVoxelPosition(int32 x, int32 y, int32 z)
{
	FVector origin = GetActorLocation();
	FVector size = m_bbContents->Bounds.GetSize();
	FVector grids(m_bbContents->GridX, m_bbContents->GridY, m_bbContents->GridZ);

	FVector localPos(size / grids);

	// Translate indexes from [0, Max) to [-Max/2, Max/2) because the origin for actors is at the center
	FVector transform(x, y, z);
	transform -= grids / 2.0f;
	transform += size / grids / 200.0f;

	// translate the calculated voxel position by origin of the actor to get the correct voxel position in the world
	return FVector(transform * localPos + origin);
}

FVector AVectronBoundingBox::getResolvedIndex(int32 index)
{
	FVector resolvedIndex(0, 0, 0);

	resolvedIndex.X = index % m_bbContents->GridX;
	resolvedIndex.Y = (index / m_bbContents->GridX) % m_bbContents->GridY;
	resolvedIndex.Z = index / (m_bbContents->GridX * m_bbContents->GridY);
	return resolvedIndex;
}

void AVectronBoundingBox::ManualUpdate()
{
	if (FVectronModule::Get().m_escrowFga == nullptr) return;
	RenderField();
}

void AVectronBoundingBox::PostEditMove(bool bFinished)
{
	ManualUpdate();
}
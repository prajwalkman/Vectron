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
	DLOG("In Cons");

	emptyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Empty SceneComponent"));

	RootComponent = emptyRoot;
}

void AVectronBoundingBox::OnConstruction(const FTransform& ft)
{
	DLOG("InOnCons");
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

	DLOG("In PreInitComp");
	
}

void AVectronBoundingBox::PostInitializeComponents()
{

	Super::PostInitializeComponents();

	DLOG("In PostInitComp");
}

// Called when the game starts or when spawned
void AVectronBoundingBox::BeginPlay()
{
	Super::BeginPlay();
}

void AVectronBoundingBox::RenderField()
{
	FlushPersistentDebugLines(GetWorld());
	TArray<AVectronPrimitive*> primitives;
	for (auto a : GetWorld()->GetCurrentLevel()->Actors)
	{
		auto c = Cast<AVectronPrimitive>(a);
		if (c) primitives.Add(c);
	}
	for (size_t i = 0; i < m_bbContents->Vectors.Num(); i++) {
		m_bbContents->Vectors[i].Normalize();
		auto ri = getResolvedIndex(i);
		auto vpos = getVoxelPosition(ri.X, ri.Y, ri.Z);
		if (m_bbContents->Vectors[i].IsNearlyZero(0.001f))
		{
			DrawDebugPoint(GetWorld(), vpos, 3.0f, FColor::Black, true);
			continue;
		}
		auto dir = m_bbContents->Vectors[i];
		for (auto p : primitives)
		{
			dir += p->fieldDirectionAtPosition(vpos);
		}
		auto vrayend = vpos + dir * 2.0f;
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

	/* Method derived from Information Theory and bit hacking (unsound?) */
	//FVector resolvedIndex(0, 0, 0);

	//int32 bitsX = (int)(floor(log2f(m_bbContents->GridX - 1)) + 1);
	//int32 bitsY = (int)(floor(log2f(m_bbContents->GridY - 1)) + 1);
	//int32 bitsZ = (int)(floor(log2f(m_bbContents->GridZ - 1)) + 1);
	////DLOG("X bits: %d, Y bits: %d, Z bits: %d", bitsX, bitsY, bitsZ);
	//int32 shifter = ((1 << bitsX) - 1);
	//resolvedIndex.X = shifter & index;
	//shifter = (((1 << bitsY) - 1) << bitsX);
	//resolvedIndex.Y = (shifter & index) >> bitsX;
	//shifter = ((1 << bitsZ) - 1) << (bitsX + bitsY);
	//resolvedIndex.Z = (shifter & index) >> (bitsX + bitsY);

	////DLOG("Resolved Index: " + resolvedIndex.ToString());
	//
	//return resolvedIndex;



	/* Simpler method */
	FVector resolvedIndex2(0, 0, 0);

	resolvedIndex2.X = index % m_bbContents->GridX;
	resolvedIndex2.Y = (index / m_bbContents->GridX) % m_bbContents->GridY;
	resolvedIndex2.Z = index / (m_bbContents->GridX * m_bbContents->GridY);
	return resolvedIndex2;
}

void AVectronBoundingBox::ManualUpdate()
{
	DLOG("MANUALLY UPDATING STUFF!");
	RenderField();
}
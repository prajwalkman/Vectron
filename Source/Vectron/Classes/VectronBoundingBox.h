// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "VectronBoundingBox.generated.h"

UCLASS()
class AVectronBoundingBox : public AActor
{
	GENERATED_UCLASS_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVectronBoundingBox();

	virtual void PostActorCreated() override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	FFGAContents* m_bbContents;

	UPROPERTY(VisibleAnywhere, Category = "FGA")
	int32 test;
	
	void setFFGAContents(FFGAContents* importedValue);

	UFUNCTION(BlueprintCallable, Category="Voxel Data")
	FVector getVoxelPosition(int32 x, int32 y, int32 z);
};

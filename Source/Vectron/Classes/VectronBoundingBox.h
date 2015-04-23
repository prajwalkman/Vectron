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

	// Called before Actor components are initialized
	virtual void PreInitializeComponents() override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	FFGAContents* m_bbContents;

	void setFFGAContents(FFGAContents* importedValue);

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Voxel Data")
	FFGAContents m_contentReference;

	UFUNCTION(BlueprintCallable, Category="Voxel Data")
	FVector getVoxelPosition(int32 x, int32 y, int32 z);

	UFUNCTION(BlueprintCallable, Category = "Voxel Data")
	FVector getResolvedIndex(int32 index);
};

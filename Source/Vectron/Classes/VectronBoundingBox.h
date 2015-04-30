// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "VectronBoundingBox.generated.h"

UCLASS()
class AVectronBoundingBox : public AActor
{
	GENERATED_UCLASS_BODY()

	void RenderField();
	
public:	

	// Called before Actor components are initialized
	virtual void PreInitializeComponents() override;

	virtual void PostInitializeComponents() override;

	virtual void OnConstruction(const FTransform&) override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	virtual bool ShouldTickIfViewportsOnly() const;

	USceneComponent* emptyRoot;

	FFGAContents* m_bbContents;

	FFGAContents* getFFGAContents();
	void setFFGAContents(FFGAContents* importedValue);

	UFUNCTION(BlueprintCallable, Category="Voxel Data")
	FVector getVoxelPosition(int32 x, int32 y, int32 z);

	UFUNCTION(BlueprintCallable, Category = "Voxel Data")
	FVector getResolvedIndex(int32 index);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voxel Data")
	float intensity = 10.0f;

	void ManualUpdate();

};

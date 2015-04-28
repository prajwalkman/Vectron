// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "VectronPrimitive.generated.h"

UCLASS()
class AVectronPrimitive : public AActor
{
	GENERATED_UCLASS_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVectronPrimitive();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called when the actor is spawned
	virtual void PreInitializeComponents() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMesh* primitiveMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	UStaticMeshComponent* primitiveMeshComponent;

	UFUNCTION(BlueprintCallable, Category = "Voxel Data")
	bool isVoxelInPrimitive(FVector voxelPosition);
	
	UFUNCTION(BlueprintCallable, Category = "Mesh")
	void reloadPrimitiveSM();

	FORCEINLINE void SetupSMComponentWithCollision(UStaticMeshComponent* comp)
	{
		if (!comp) return;

		comp->bOwnerNoSee = false;
		comp->bCastDynamicShadow = false;
		comp->CastShadow = false;
		comp->BodyInstance.SetObjectType(ECC_WorldDynamic);
		comp->BodyInstance.SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		comp->BodyInstance.SetResponseToAllChannels(ECR_Ignore);
		comp->BodyInstance.SetResponseToChannel(ECC_WorldStatic, ECR_Block);
		comp->BodyInstance.SetResponseToChannel(ECC_WorldDynamic, ECR_Block);
		comp->BodyInstance.SetResponseToChannel(ECC_Pawn, ECR_Block);
		comp->BodyInstance.bSimulatePhysics = false;
		comp->bGenerateOverlapEvents = true;
	}
};

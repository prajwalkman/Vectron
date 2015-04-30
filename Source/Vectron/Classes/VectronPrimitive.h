// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "VectronPrimitive.generated.h"

UENUM(BlueprintType)
enum class EPrimitiveActorType : uint8 
{
	PAT_CONSTANT		UMETA(DisplayName = "Constant"),
	PAT_ATTRACTIVE		UMETA(DisplayName = "Attractive"),
	PAT_REPULSIVE		UMETA(DisplayName = "Repulsive")
};

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
	virtual void Tick(float DeltaSeconds) override;

	// Called when the actor is spawned
	virtual void PreInitializeComponents() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void PostEditMove(bool bFinished) override;

	~AVectronPrimitive();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voxel Data")
	EPrimitiveActorType primitiveType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voxel Data")
	FVector constantForce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voxel Data")
	float intensity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Voxel Data")
	UStaticMeshComponent* primitiveMeshComponent;

	UFUNCTION(BlueprintCallable, Category = "Voxel Data")
	bool isVoxelInPrimitive(FVector voxelPosition);

	UFUNCTION(BlueprintCallable, Category = "Voxel Data")
	FVector fieldDirectionAtPosition(FVector voxelPosition);

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

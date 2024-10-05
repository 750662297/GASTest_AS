// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GTProjectileBase.h"
#include "GameplayEffect.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GTTempProjectile.generated.h"

UCLASS()
class GASTEST_AS_API AGTTempProjectile : public AGTProjectileBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGTTempProjectile();
    
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="GASTest|TempProjectile")
	class USphereComponent* Collision;
    
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="GASTest|TempProjectile")
	class UStaticMeshComponent* Sphere;
    
protected:

	UPROPERTY(BlueprintReadWrite)
	FHitResult FinalHit;
    
	UFUNCTION()
	void OnComBeginOverlap( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
    
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};

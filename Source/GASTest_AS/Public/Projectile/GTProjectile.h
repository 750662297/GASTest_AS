// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffect.h"
#include "GTProjectileBase.h"
#include "GTProjectile.generated.h"

UCLASS()
class GASTEST_AS_API AGTProjectile : public AGTProjectileBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGTProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};

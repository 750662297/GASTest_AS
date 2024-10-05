// Fill out your copyright notice in the Description page of Project Settings.

#include "Projectile/GTProjectileBase.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AGTProjectileBase::AGTProjectileBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(FName("ProjectileMovement"));
}

// Called when the game starts or when spawned
void AGTProjectileBase::BeginPlay()
{
	Super::BeginPlay();
    
}

FHitResult AGTProjectileBase::HandleResult(FHitResult result)
{
	result.bStartPenetrating = false;

	return result;
}
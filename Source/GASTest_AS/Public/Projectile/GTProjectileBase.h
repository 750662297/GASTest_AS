// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffect.h"
#include "GTProjectileBase.generated.h"

UCLASS()
class GASTEST_AS_API AGTProjectileBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGTProjectileBase();
    
	//ExposeOnSpawn 元数据标签，设置为true时，允许该属性在生成Actor时通过蓝图或其他代码进行覆盖
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Meta = (ExposeOnSpawn = true))
	float Range;

	UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
	FGameplayEffectSpecHandle DamageEffectSpecHandle;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UFUNCTION(BlueprintCallable)
	FHitResult HandleResult(FHitResult result);
    
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};

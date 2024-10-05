// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "GTDamageTextWidgetComponent.generated.h"

/**
 * 
 */
UCLASS()
class GASTEST_AS_API UGTDamageTextWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetDamageText(float Damage);
};

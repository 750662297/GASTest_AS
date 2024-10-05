// Fill out your copyright notice in the Description page of Project Settings.
// 伤害计算

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GTDamageExecCalculation.generated.h"

/**
 *
 */
UCLASS()
class GASTEST_AS_API UGTDamageExecCalculation : public UGameplayEffectExecutionCalculation
{
    GENERATED_BODY()

public:
    UGTDamageExecCalculation();

    virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};

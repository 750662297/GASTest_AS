﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GTDamageRateExecCalculation.generated.h"

/**
 * 
 */
UCLASS()
class GASTEST_AS_API UGTDamageRateExecCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

	UGTDamageRateExecCalculation();
    
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

};

// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Abilities/GTDamageRateExecCalculation.h"
#include "Characters/Abilities/AttributeSets/GTAttributeSetBase.h"

struct GTDamageRateStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageMultiplier);

	GTDamageRateStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UGTAttributeSetBase, DamageMultiplier, Source, true)
	}
};

static const GTDamageRateStatics& DamageRateStatics()
{
	static GTDamageRateStatics DStatics;
	return DStatics;
}

UGTDamageRateExecCalculation::UGTDamageRateExecCalculation()
{
	RelevantAttributesToCapture.Add(DamageRateStatics().DamageMultiplierDef);
}
    
void UGTDamageRateExecCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParamters;
	EvaluationParamters.SourceTags = SourceTags;

	float DamageRate = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageRateStatics().DamageMultiplierDef,EvaluationParamters,DamageRate);
	DamageRate += Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.DamageRate")), false, 0.0f);
	
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageRateStatics().DamageMultiplierProperty, EGameplayModOp::Override, DamageRate));
}
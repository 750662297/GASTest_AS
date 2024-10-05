// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Abilities/GTDamageExecCalculation.h"

#include "Characters/Abilities/GTAbilitySystemComponent.h"
#include "Characters/Abilities/AttributeSets/GTAttributeSetBase.h"

struct GTDamageStatics
{
    DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
    DECLARE_ATTRIBUTE_CAPTUREDEF(Damage)

    GTDamageStatics()
    {
        // 定义具体的属性捕获
        DEFINE_ATTRIBUTE_CAPTUREDEF(UGTAttributeSetBase, Damage, Source, true);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UGTAttributeSetBase, Armor, Source, false);
    }
};

static const GTDamageStatics& DamageStatics()
{
    static GTDamageStatics DStatics;
    return DStatics;
}

UGTDamageExecCalculation::UGTDamageExecCalculation()
{
    // 添加相关属性到捕获列表中
    //  RelevantAttributesToCapture时一个数组，用于存储需要捕获的相关属性（在游戏中应用效果时需要捕获和处理的属性）
    RelevantAttributesToCapture.Add(DamageStatics().DamageDef);
    RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
}

void UGTDamageExecCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                                      FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    // OutExecutionOutput 储存计算后的输出值

    UAbilitySystemComponent* TargetAbilitySystemComponent  = ExecutionParams.GetTargetAbilitySystemComponent();
    UAbilitySystemComponent* SourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();

    AActor* SourceActor = SourceAbilitySystemComponent ? SourceAbilitySystemComponent->GetAvatarActor() : nullptr;
    AActor* TargetActor = TargetAbilitySystemComponent ? TargetAbilitySystemComponent->GetAvatarActor() : nullptr;

    const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

    // 获取tags
    // GetAggregatedTags GameplayTagCountContainer 类的一个方法，它返回一个 FGameplayTagContainer
    // 对象，该对象包含了所有捕获的标签，但不包含计数信息（将所有标签汇总到一个容器中然后返回）
    const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

    FAggregatorEvaluateParameters EvaluationParameters;
    EvaluationParameters.SourceTags = SourceTags;
    EvaluationParameters.TargetTags = TargetTags;

    // 根据双方tags，计算护甲和最终伤害
    float Armor = 0.0f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters, Armor);
    Armor = FMath::Max<float>(Armor, 0.0f);

    float Damage = 0.0f;
    // Capture optional damage value set on the damage GE as a CalculationModifier under the ExecutionCalculation
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageDef, EvaluationParameters, Damage);
    // Add SetByCaller damage if it exists
    Damage += FMath::Max<float>(Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), false, -1.0f), 0.0f);

    float UnmitigatedDamage = Damage* SourceAbilitySystemComponent->GetNumericAttribute(UGTAttributeSetBase::GetDamageMultiplierAttribute()); // Can multiply any damage boosters here
	
    float MitigatedDamage = (UnmitigatedDamage) * (100 / (100 + Armor));

    if (MitigatedDamage > 0.f)
    {
        // Set the Target's damage meta attribute
        OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().DamageProperty, EGameplayModOp::Additive, MitigatedDamage));
    }

    // Broadcast damages to Target ASC
    UGTAbilitySystemComponent* TargetASC = Cast<UGTAbilitySystemComponent>(TargetAbilitySystemComponent);
    if (TargetASC)
    {
        UGTAbilitySystemComponent* SourceASC = Cast<UGTAbilitySystemComponent>(SourceAbilitySystemComponent);
        TargetASC->ReceiveDamage(SourceASC, UnmitigatedDamage, MitigatedDamage);
    }
}

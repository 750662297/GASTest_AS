// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GTGameplayAbitity.h"
#include "GTGA_CharacterJump.generated.h"

/**
 *
 */
UCLASS()
class GASTEST_AS_API UGTGA_CharacterJump : public UGTGameplayAbitity
{
	GENERATED_BODY()
public:
	UGTGA_CharacterJump();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo,
								 const FGameplayAbilityActivationInfo ActivationInfo,
								 const FGameplayEventData            *TriggerEventData) override;
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo,
									const FGameplayTagContainer *SourceTags, const FGameplayTagContainer *TargetTags,
									FGameplayTagContainer *OptionalRelevantTags) const override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo,
							   const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo,
							   const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
};

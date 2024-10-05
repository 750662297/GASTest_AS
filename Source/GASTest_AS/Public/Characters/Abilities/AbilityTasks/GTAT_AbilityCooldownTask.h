// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "GTAT_AbilityCooldownTask.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCooldownEnd, FGameplayTag, AbilityTag);


/**
 * 
 */
UCLASS()
class GASTEST_AS_API UGTAT_AbilityCooldownTask : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UGTAT_AbilityCooldownTask* CreateCooldownTask(UGameplayAbility* OwningAbility, float CooldownDuration,FGameplayTag AbilityTag);

	virtual void Activate() override;

	UPROPERTY(BlueprintAssignable)
	FOnCooldownEnd OnCooldownEnd;

private:
	float CooldownDuration;
	FGameplayTag AbilityTag;
	FTimerHandle TimerHandle;

	void OnTimerFinished();
};

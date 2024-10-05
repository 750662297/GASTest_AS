// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Characters/Abilities/GTGameplayAbility.h"
#include "CoreMinimal.h"
#include "GameplayTags.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "GTGA_TempAbility.generated.h"

/**
 *
 */
UCLASS()
class GASTEST_AS_API UGTGA_TempAbility : public UGTGameplayAbility
{
	GENERATED_BODY()

public:
	UGTGA_TempAbility();
	
	UPROPERTY(BlueprintReadOnly,EditAnywhere, Category = "GASTest|Ability")
	TSubclassOf<UGameplayEffect> DamageGameplayEffect;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	
protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASTest|Ability")
	float Duration; //持续时间

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASTest|Ability")
	float Rate; //提升比率
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASTest|Ability")
	float MaxRate; //可以提升到的最大比率

	UPROPERTY(BlueprintReadOnly,EditAnywhere, Category = "GASTest|Ability")
	int32 MaxNumber;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GASTest|Ability")
	FGameplayTag HitTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GASTest|Ability")
	FGameplayTag Ability2Tag;

	UFUNCTION()
	void EventReceived(FGameplayEventData Payload);

	void OnCooldownEnd(FGameplayTag AbilityTag) override;

	UFUNCTION()
	void UpdateBufferNumber(FGameplayTag AbilityTag, int32 Number);
	
private:
	void RecoverEffectWhenEnd();

	UAbilityTask_WaitGameplayEvent* WaitEventTask;
	
	FTimerHandle TimerHandle_EndAbility; // 声明 TimerHandle
	int32 Record; //计数
	
};

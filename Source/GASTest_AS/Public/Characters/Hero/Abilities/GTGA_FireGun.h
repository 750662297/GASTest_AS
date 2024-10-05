// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile/GTProjectileBase.h"
#include "..\..\Abilities\GTGameplayAbitity.h"
#include "GTGA_FireGun.generated.h"

/**
 * 
 */
UCLASS()
class GASTEST_AS_API UGTGA_FireGun : public UGTGameplayAbitity
{
	GENERATED_BODY()

public:
	UGTGA_FireGun();

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UAnimMontage* FireHipMontage;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UAnimMontage* FireIronsightsMontage;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<AGTProjectileBase> ProjectileClass;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UGameplayEffect> DamageGameplayEffect;

	/** Actually activate ability, do not call this directly. We'll call it from APAHeroCharacter::ActivateAbilitiesWithTags(). */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData *TriggerEventData) override;

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float Range;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float Damage;

	UFUNCTION()
	void OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void EventReceived(FGameplayTag EventTag, FGameplayEventData EventData);
};

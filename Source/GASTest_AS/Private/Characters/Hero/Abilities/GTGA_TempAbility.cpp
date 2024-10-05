// Fill out your copyright notice in the Description page of Project Settings.
// Copyright 2023 Dan Kestranek.

#include "Characters/Hero/Abilities/GTGA_TempAbility.h"
#include "Characters/Abilities/GTAbilitySystemComponent.h"
#include "Characters/GTCharacterBase.h"
#include "Characters/Abilities/AttributeSets/GTAttributeSetBase.h"
#include "Internationalization/Text.h"
#include "Player/GTPlayerController.h"
#include "UI/GTHUDWidget.h"

UGTGA_TempAbility::UGTGA_TempAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	Ability2Tag = FGameplayTag::RequestGameplayTag(FName("Ability.Buff.Buff1"));
	AbilityTags.AddTag(Ability2Tag);
	ActivationOwnedTags.AddTag(Ability2Tag);

	// ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill")));

	HitTag = FGameplayTag::RequestGameplayTag(FName("Damage.Hit"));

	Duration = 10.0f;
	Rate = 0.1f;
	// MaxRate = 1.5f;
	MaxNumber = 50;
	Record = 0;
	AbilityName = NSLOCTEXT("UGTGA_TempAbility","Greeting", "Temp Ability");
}

void UGTGA_TempAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo,
                                        const FGameplayAbilityActivationInfo ActivationInfo,
                                        const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	if (GetOwningActorFromActorInfo()->GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, HitTag);
	if (WaitEventTask)
	{
		UE_LOG(LogTemp, Warning, TEXT("WaitEventTask Created and Activated"));
		WaitEventTask->EventReceived.AddDynamic(this, &UGTGA_TempAbility::EventReceived);
		WaitEventTask->Activate();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("WaitEventTask Creation Failed"));
	}

	StartCooldown(Duration, Ability2Tag);

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UGTGA_TempAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
                                   bool bWasCancelled)
{
	if (WaitEventTask)
	{
		WaitEventTask->EventReceived.RemoveDynamic(this, &UGTGA_TempAbility::EventReceived);
		WaitEventTask->EndTask();
	}

	WaitEventTask = nullptr;
	RecoverEffectWhenEnd();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}


void UGTGA_TempAbility::EventReceived(FGameplayEventData Payload)
{
	UE_LOG(LogTemp, Warning, TEXT("Event Received"));
	
	FGameplayTag EventTag = Payload.EventTag;

	if (!EventTag.IsValid() || EventTag != HitTag)
	{
		return;
	}

	if (GetOwningActorFromActorInfo()->GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	AGTCharacterBase* Hero = Cast<AGTCharacterBase>(GetAvatarActorFromActorInfo());
	if (!Hero)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	UGTAbilitySystemComponent* ASC = Cast<UGTAbilitySystemComponent>(Hero->GetAbilitySystemComponent());
	if (ASC)
	{
		Record++;
		if (Record <= MaxNumber)
		{
			FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(
				DamageGameplayEffect, GetAbilityLevel());
			SpecHandle.Data.Get()->
			           SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.DamageRate")), Rate);

			FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
			FActiveGameplayEffectHandle ActiveHandle = ASC->ApplyGameplayEffectSpecToSelf(*Spec);
			
			UpdateBufferNumber(Ability2Tag,Record);
		}

	}
}

void UGTGA_TempAbility::RecoverEffectWhenEnd()
{

	AGTCharacterBase* Hero = Cast<AGTCharacterBase>(GetAvatarActorFromActorInfo());
	if (!Hero)
	{
		return;
	}

	UGTAbilitySystemComponent* ASC = Cast<UGTAbilitySystemComponent>(Hero->GetAbilitySystemComponent());
	if (ASC)
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DamageGameplayEffect, GetAbilityLevel());
		SpecHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.DamageRate")),
		                                               -(FMath::Min(Record, MaxNumber) * Rate));

		FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
		FActiveGameplayEffectHandle ActiveHandle = ASC->ApplyGameplayEffectSpecToSelf(*Spec);
		
		Record =0;
	}

}

void UGTGA_TempAbility::OnCooldownEnd(FGameplayTag AbilityTag)
{
	Super::OnCooldownEnd(AbilityTag);

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGTGA_TempAbility::UpdateBufferNumber(FGameplayTag AbilityTag, int32 Number)
{
	AGTCharacterBase* Hero = Cast<AGTCharacterBase>(GetAvatarActorFromActorInfo());
	if(!Hero)
	{
		return;
	}
	
	AGTPlayerController* PlayerController = Cast<AGTPlayerController>(Hero->GetController());
	if(PlayerController && PlayerController->GetHUD())
	{
		UGTHUDWidget* HUD = PlayerController->GetHUD();
		
		HUD->UpdateBufferNumber(AbilityTag,Number);
	}
}

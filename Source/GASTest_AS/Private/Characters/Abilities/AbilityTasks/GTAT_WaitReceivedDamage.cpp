// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/Abilities/AbilityTasks/GTAT_WaitReceivedDamage.h"

#include "Characters/Abilities/GTAbilitySystemComponent.h"

UGTAT_WaitReceivedDamage::UGTAT_WaitReceivedDamage(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer)
{
	TriggerOnce = false;
}

UGTAT_WaitReceivedDamage *UGTAT_WaitReceivedDamage::WaitReceivedDamage(UGameplayAbility *OwningAbility, bool TriggerOnce)
{
	UGTAT_WaitReceivedDamage *MyObj = NewAbilityTask<UGTAT_WaitReceivedDamage>(OwningAbility);
	MyObj->TriggerOnce              = TriggerOnce;

	return MyObj;
}

void UGTAT_WaitReceivedDamage::Activate()
{
	UGTAbilitySystemComponent* GTASC = Cast<UGTAbilitySystemComponent>(AbilitySystemComponent);

	if(GTASC)
	{
		GTASC->ReceivedDamage.AddDynamic(this, &UGTAT_WaitReceivedDamage::OnDamageReceived);
	}
}

void UGTAT_WaitReceivedDamage::OnDestroy(bool AbilityIsEnding)
{
	UGTAbilitySystemComponent* GTASC = Cast<UGTAbilitySystemComponent>(AbilitySystemComponent);

	if(GTASC)
	{
		GTASC->ReceivedDamage.RemoveDynamic(this, &UGTAT_WaitReceivedDamage::OnDamageReceived);
	}

	Super::OnDestroy(AbilityIsEnding);
}


void UGTAT_WaitReceivedDamage::OnDamageReceived(UGTAbilitySystemComponent *SourceASC, float UnmitigatedDamage,
												float MitigatedDamage)
{
	if(ShouldBroadcastAbilityTaskDelegates())
	{
		OnDamage.Broadcast(SourceASC, UnmitigatedDamage, MitigatedDamage);
	}

	if(TriggerOnce)
	{
		EndTask();
	}
}

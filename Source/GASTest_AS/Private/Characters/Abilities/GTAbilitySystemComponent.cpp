// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Abilities/GTAbilitySystemComponent.h"


void UGTAbilitySystemComponent::ReceiveDamage(UGTAbilitySystemComponent* SourceASC, float UnmitigatedDamage, float MitigatedDamage)
{
	ReceivedDamage.Broadcast(SourceASC,UnmitigatedDamage,MitigatedDamage);
} 
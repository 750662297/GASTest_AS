// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Abilities/AbilityTasks/GTAT_AbilityCooldownTask.h"

UGTAT_AbilityCooldownTask* UGTAT_AbilityCooldownTask::CreateCooldownTask(UGameplayAbility* OwningAbility, float CooldownDuration, FGameplayTag AbilityTag)
{
	UGTAT_AbilityCooldownTask* Task = NewAbilityTask<UGTAT_AbilityCooldownTask>(OwningAbility);
	Task->CooldownDuration = CooldownDuration;
	Task->AbilityTag = AbilityTag;

	return Task;
}

void UGTAT_AbilityCooldownTask::Activate()
{
	if(GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UGTAT_AbilityCooldownTask::OnTimerFinished,CooldownDuration,false);
	}
}

void UGTAT_AbilityCooldownTask::OnTimerFinished()
{
	OnCooldownEnd.Broadcast(AbilityTag);
	EndTask();
}


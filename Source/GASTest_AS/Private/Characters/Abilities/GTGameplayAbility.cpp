// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Abilities/GTGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "Characters/GTCharacterBase.h"
#include "Player/GTPlayerController.h"
#include "UI/GTHUDWidget.h"
#include "Characters/Abilities/AbilityTasks/GTAT_AbilityCooldownTask.h"

UGTGameplayAbility::UGTGameplayAbility()
{
	//默认设置为每个Actor实例化一份能力，确保每个角色的能力状态相互独立，互不影响
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	//默认添加两个阻止技能激活的标签（死亡，击晕状态下无法释放技能）
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Dead")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun")));
}

void UGTGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilitySpec &Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	//若ActivateAbilityOnGranted为true，则立刻激活该能力，通常用于需要被赋予时立即生效的能力（被动技能，某些情况下需要立即执行的效果）
	if(ActivateAbilityOnGranted)
	{
		ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle,false);
	}
}


void UGTGameplayAbility::StartCooldown(float CooldownDuration, FGameplayTag AbilityTag)
{
	UGTAT_AbilityCooldownTask* CooldownTask = UGTAT_AbilityCooldownTask::CreateCooldownTask(this, CooldownDuration,AbilityTag);
	CooldownTask->OnCooldownEnd.AddDynamic(this, &UGTGameplayAbility::OnCooldownEnd);

	CooldownTask->Activate();

	AGTCharacterBase* Hero = Cast<AGTCharacterBase>(GetAvatarActorFromActorInfo());
	if(!Hero)
	{
		return;
	}
	
	AGTPlayerController* PlayerController = Cast<AGTPlayerController>(Hero->GetController());
	if(PlayerController && PlayerController->GetHUD())
	{
		UGTHUDWidget* HUD = PlayerController->GetHUD();

		HUD->AddCooldownInfo(AbilityName,AbilityTag,CooldownDuration,0);
	}
}

void UGTGameplayAbility::OnCooldownEnd(FGameplayTag AbilityTag)
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
		
		HUD->RemoveCooldown(AbilityTag);
	}
}
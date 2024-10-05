// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Abilities/GTGameplayAbitity.h"
#include "AbilitySystemComponent.h"

UGTGameplayAbitity::UGTGameplayAbitity()
{
	//默认设置为每个Actor实例化一份能力，确保每个角色的能力状态相互独立，互不影响
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	//默认添加两个阻止技能激活的标签（死亡，击晕状态下无法释放技能）
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Dead")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun")));
}

void UGTGameplayAbitity::OnAvatarSet(const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilitySpec &Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	//若ActivateAbilityOnGranted为true，则立刻激活该能力，通常用于需要被赋予时立即生效的能力（被动技能，某些情况下需要立即执行的效果）
	if(ActivateAbilityOnGranted)
	{
		ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle,false);
	}
}

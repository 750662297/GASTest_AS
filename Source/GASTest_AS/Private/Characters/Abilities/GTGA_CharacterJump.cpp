// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/Abilities/GTGA_CharacterJump.h"
#include "Characters/GTCharacterBase.h"

UGTGA_CharacterJump::UGTGA_CharacterJump()
{
    AbilityInputID   = EGTAbilityInputID::Jump;
    InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;
    AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Jump")));
}

void UGTGA_CharacterJump::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo,
                                          const FGameplayAbilityActivationInfo ActivationInfo,
                                          const FGameplayEventData            *TriggerEventData)
{
    // HasAuthorityOrPredictionKey 检查当前 Actor
    // 是否在具有权威的机器上运行，或者当前客户端是否拥有正确的预测密钥来进行预测性执行
    if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
    {
        if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
        {
            EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        }

        ACharacter *Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
        Character->Jump();
    }
}

bool UGTGA_CharacterJump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                             const FGameplayAbilityActorInfo *ActorInfo,
                                             const FGameplayTagContainer *SourceTags, const FGameplayTagContainer *TargetTags,
                                             FGameplayTagContainer *OptionalRelevantTags) const
{
    if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
    {
        return false;
    }

    const AGTCharacterBase *Character =
        CastChecked<AGTCharacterBase>(ActorInfo->AvatarActor.Get(), ECastCheckedType::NullAllowed);
    return Character && Character->CanJump();
}

void UGTGA_CharacterJump::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo,
                                        const FGameplayAbilityActivationInfo ActivationInfo)
{
    if (ActorInfo != NULL && ActorInfo->AvatarActor != NULL)
    {
        CancelAbility(Handle, ActorInfo, ActivationInfo, true);
    }
}

void UGTGA_CharacterJump::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo,
                                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
    //当范围锁正在生效时，取消技能的请求会被暂时延迟
    if(ScopeLockCount >0)
    {
        //WaitingToExecute 存储待执行委托，在所有范围锁都被释放后依次执行
        //FPostLockDelegate 用于封装需要在锁释放后执行的函数调用
        WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &UGTGA_CharacterJump::CancelAbility, Handle,ActorInfo,ActivationInfo,bReplicateCancelAbility));
        return;
    }

    Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

    ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
    Character->StopJumping();
}


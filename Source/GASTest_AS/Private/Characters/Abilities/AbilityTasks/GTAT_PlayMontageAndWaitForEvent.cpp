// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/Abilities/AbilityTasks/GTAT_PlayMontageAndWaitForEvent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameFramework/Character.h"

UGTAT_PlayMontageAndWaitForEvent::UGTAT_PlayMontageAndWaitForEvent(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
    Rate                 = 1.f;
    bStopWhenAbilityEnds = true;
}

void UGTAT_PlayMontageAndWaitForEvent::OnMontageBlendingOut(UAnimMontage *Montage, bool bInterrupted)
{
    if (Ability && Ability->GetCurrentMontage() == MontageToPlay)
    {
        if (Montage == MontageToPlay)
        {
            // 清除当前正在播放的动画技能
            AbilitySystemComponent->ClearAnimatingAbility(Ability);

            // 重置 AnimRootMotionTranslationScale

            /*
             * ROLE_AutonomousProxy 由客户端控制的角色，但该角色的状态是由服务器最终决定的
             * LocalPredicted
             *表示能力在网络上的执行方式是本地预测，服务器验证的(客户端在接收到输入时立即执行能力，并显示预测的结果；但客户端将输入发送到服务器，服务器验证这些输入并最终决定能力的效果。如果客户端的预测与服务器的实际结果不一致，服务器会进行校正）
             *为什么在角色为ROLE_AutonomousProxy时，要保证网络执行策略为LocalPredicted才进行下一步：
             *1、确保客户端和服务器之间的状态一致。如果能力不是本地预测的，那么客户端和服务器之间可能会出现状态不一致的情况，导致动画或其他行为出现问题；
             *2、通过本地预测，客户端可以立即响应玩家的输入，而不需要等待服务器的确认。这可以显著提高游戏的响应性和流畅度
             *3、防止作弊：虽然客户端可以自主控制角色，但最终的状态是由服务器验证的。如果能力不是本地预测的，那么客户端可能会尝试执行一些非法的操作，服务器可以通过验证来防止这种情况（多人网络游戏情况，实际实施时需要进一步考虑）
             *
             */
            ACharacter *Character = Cast<ACharacter>(GetAvatarActor());
            if (Character
                && (Character->GetLocalRole() == ROLE_Authority
                    || (Character->GetLocalRole() == ROLE_AutonomousProxy
                        && Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
            {
                Character->SetAnimRootMotionTranslationScale(1.f);
            }
        }
    }

    if (bInterrupted)
    {
        // 如果蒙太奇被中断

        // ShouldBroadcastAbilityTaskDelegates 判断是否进行广播，set函数为SetShouldBroadcastAbilityTaskDelegates

        if (ShouldBroadcastAbilityTaskDelegates())
        {
            OnInterrupted.Broadcast(FGameplayTag(), FGameplayEventData());
        }
    }
    else
    {
        // 如果蒙太奇正常混合退出
        if (ShouldBroadcastAbilityTaskDelegates())
        {
            OnBlendOut.Broadcast(FGameplayTag(), FGameplayEventData());
        }
    }
}

void UGTAT_PlayMontageAndWaitForEvent::OnAbilityCancelled()
{
    if (StopPlayingMontage())
    {
        if (ShouldBroadcastAbilityTaskDelegates())
        {
            OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
        }
    }
}

void UGTAT_PlayMontageAndWaitForEvent::OnMontageEnded(UAnimMontage *Montage, bool bInterrupted)
{
    if (!bInterrupted)
    {
        if (ShouldBroadcastAbilityTaskDelegates())
        {
            OnCompleted.Broadcast(FGameplayTag(), FGameplayEventData());
        }
    }

    EndTask();
}

void UGTAT_PlayMontageAndWaitForEvent::OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData *Payload)
{
    if (ShouldBroadcastAbilityTaskDelegates())
    {
        FGameplayEventData TempData = *Payload;
        TempData.EventTag           = EventTag;

        EventReceived.Broadcast(EventTag, TempData);
    }
}

UGTAT_PlayMontageAndWaitForEvent *UGTAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(
    UGameplayAbility *OwningAbility, FName TaskInstanceName, UAnimMontage *MontageToPlay, FGameplayTagContainer EventTags,
    float Rate, FName StartSection, bool bStopWhenAbilityEnds, float AnimRootMotionTranslationScale)
{
    UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Rate(Rate);

    UGTAT_PlayMontageAndWaitForEvent* MyObj = NewAbilityTask<UGTAT_PlayMontageAndWaitForEvent>(OwningAbility, TaskInstanceName);
    MyObj->MontageToPlay = MontageToPlay;
    MyObj->EventTags = EventTags;
    MyObj->Rate = Rate;
    MyObj->StartSection = StartSection;
    MyObj->AnimRootMotionTranslationScale = AnimRootMotionTranslationScale;
    MyObj->bStopWhenAbilityEnds = bStopWhenAbilityEnds;

    return MyObj;
}

void UGTAT_PlayMontageAndWaitForEvent::Activate()
{
    if(Ability == nullptr)
    {
        return;
    }

    bool bPlayedMontage = false;
    if(AbilitySystemComponent.IsValid())
    {
        const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
        UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
        if(AnimInstance!=nullptr)
        {
            //Bind to event callback
            EventHandle = AbilitySystemComponent->AddGameplayEventTagContainerDelegate(EventTags, FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &UGTAT_PlayMontageAndWaitForEvent::OnGameplayEvent));

            if(AbilitySystemComponent->PlayMontage(Ability, Ability->GetCurrentActivationInfo(),MontageToPlay, Rate, StartSection) >0.f)
            {
                if(!ShouldBroadcastAbilityTaskDelegates())
                {
                    return;
                }

                CancelledHandle = Ability->OnGameplayAbilityCancelled.AddUObject(this, &UGTAT_PlayMontageAndWaitForEvent::OnAbilityCancelled);

                //绑定Montage结束和混合结束的回调
                BlendingOutDelegate.BindUObject(this, &UGTAT_PlayMontageAndWaitForEvent::OnMontageBlendingOut);
                AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, MontageToPlay);

                MontageEndedDelegate.BindUObject(this, &UGTAT_PlayMontageAndWaitForEvent::OnMontageEnded);
                AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);

                ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
                if(Character && (Character->GetLocalRole() == ROLE_Authority ||
                    (Character->GetLocalRole() == ROLE_AutonomousProxy && Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
                {
                    Character->SetAnimRootMotionTranslationScale(AnimRootMotionTranslationScale);
                }

                bPlayedMontage = true;
            }
        }

        else
        {
            UE_LOG(LogTemp, Warning, TEXT("UGTAbilityTask_PlayMontageAndWaitForEvent call to PlayMontage failed!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("UGTAbilityTask_PlayMontageAndWaitForEvent called on invalid AbilitySystemComponent"));
    }

    if(!bPlayedMontage)
    {
        UE_LOG(LogTemp, Warning,
               TEXT("UGTAbilityTask_PlayMontageAndWaitForEvent called in Ability %s failed to play montage %s; Task Instance Name %s."), *Ability->GetName(), *GetNameSafe(MontageToPlay), *InstanceName.ToString());

        if (ShouldBroadcastAbilityTaskDelegates())
        {
            OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
        }
    }

    //设置任务为等待Avatar状态
    SetWaitingOnAvatar();
}

void UGTAT_PlayMontageAndWaitForEvent::ExternalCancel()
{
    if(!AbilitySystemComponent.IsValid())
    {
        return;
    }

    OnAbilityCancelled();
    Super::ExternalCancel();
}

void UGTAT_PlayMontageAndWaitForEvent::OnDestroy(bool AbilityEnded)
{
    /*
     *为什么只处理了取消和事件的委托？
     *
     * Montage结束和混合结束的委托：通常在Montage播放完毕后自动解除绑定。如果 Montage 已经播放完毕，这些委托自然会被移除。
     * 如果 Montage 还在播放，那么在 OnDestroy 中调用 StopPlayingMontage 会停止 Montage 并移除这些委托
     */
    
    if(Ability)
    {
        Ability->OnGameplayAbilityCancelled.Remove(CancelledHandle);
        
        if(AbilityEnded && bStopWhenAbilityEnds)
        {
            StopPlayingMontage();
        }
    }

    if(AbilitySystemComponent.IsValid())
    {
        AbilitySystemComponent->RemoveGameplayEventTagContainerDelegate(EventTags,EventHandle);
    }
}

bool UGTAT_PlayMontageAndWaitForEvent::StopPlayingMontage()
{
    const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
    if(!ActorInfo)
    {
        return false;
    }

    UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
    if(AnimInstance == nullptr)
    {
        return false;
    }

    if(AbilitySystemComponent.IsValid() && Ability)
    {
        if(AbilitySystemComponent->GetAnimatingAbility() == Ability
            && AbilitySystemComponent->GetCurrentMontage() == MontageToPlay)
        {

            FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(MontageToPlay);
            if(MontageInstance)
            {
                MontageInstance->OnMontageBlendingOutStarted.Unbind();
                MontageInstance->OnMontageEnded.Unbind();
            }

            AbilitySystemComponent->CurrentMontageStop();
            return true;
        }
    }

    return  false;
}

FString UGTAT_PlayMontageAndWaitForEvent::GetDebugString() const
{
    UAnimMontage* PlayingMontage = nullptr;
    if(Ability)
    {
        const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
        UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();

        if(AnimInstance !=nullptr)
        {
            PlayingMontage = AnimInstance->Montage_IsActive(MontageToPlay) ? MontageToPlay  : AnimInstance->GetCurrentActiveMontage();
        }
    }

    
    return FString::Printf(TEXT("PlayMontageAndWaitForEvent. MontageToPlay: %s  (Currently Playing): %s"), *GetNameSafe(MontageToPlay), *GetNameSafe(PlayingMontage));
}

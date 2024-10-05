// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "GTAT_PlayMontageAndWaitForEvent.generated.h"

class UGTAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGTPlayMontageAndWaitForEventDelegate, FGameplayTag, EventTag,
                                             FGameplayEventData, EventData);

/**
 *
 */
UCLASS() class GASTEST_AS_API UGTAT_PlayMontageAndWaitForEvent : public UAbilityTask
{
    GENERATED_BODY()

public:
    UGTAT_PlayMontageAndWaitForEvent(const FObjectInitializer& ObjectInitializer);

    virtual void    Activate() override;                    // 激活任务的方法
    virtual void    ExternalCancel() override;              // 外部取消任务的方法
    virtual FString GetDebugString() const override;        // 返回调试字符串的方法
    virtual void    OnDestroy(bool AbilityEnded) override;  // 销毁任务的方法

    /** The montage completely finished playing 蒙太奇完全播放完毕时调用 */
    UPROPERTY(BlueprintAssignable)
    FGTPlayMontageAndWaitForEventDelegate OnCompleted;

    /** The montage started blending out 蒙太奇开始混合推出时调用*/
    UPROPERTY(BlueprintAssignable)
    FGTPlayMontageAndWaitForEventDelegate OnBlendOut;

    /** The montage was interrupted 蒙太奇被中断时调用*/
    UPROPERTY(BlueprintAssignable)
    FGTPlayMontageAndWaitForEventDelegate OnInterrupted;

    /** The ability task was explicitly cancelled by another ability 任务被显式取消时调用*/
    UPROPERTY(BlueprintAssignable)
    FGTPlayMontageAndWaitForEventDelegate OnCancelled;

    /** One of the triggering gameplay events happened 当匹配的gameplay事件发生时被调用*/
    UPROPERTY(BlueprintAssignable)
    FGTPlayMontageAndWaitForEventDelegate EventReceived;

    /*
     *用于创建并初始化这个任务的静态函数，接受多个参数，包括拥有该任务的能力，任务实例名称，要播放的蒙太奇，事件标签容器，播放速率等
     *MontageToPlay 要播放的蒙太奇动画
     *EventTags  需要匹配的游戏事件标签容器
     *Rate 播放速率
     *StartSectioin 要播放的段落名称
     *AnimRootMotionTranslationScale 根运动平移缩放
     *bStopWhenAbilityEnd 如果为true，则在能力结束时停止蒙太奇
     *
     */
    UFUNCTION(BlueprintCallable, Category = "Ability|Tasks",
              meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
    static UGTAT_PlayMontageAndWaitForEvent* PlayMontageAndWaitForEvent(UGameplayAbility* OwningAbility,
                                                                        FName TaskInstanceName, UAnimMontage* MontageToPlay,
                                                                        FGameplayTagContainer EventTags, float Rate = 1.f,
                                                                        FName StartSection                   = NAME_None,
                                                                        bool  bStopWhenAbilityEnds           = true,
                                                                        float AnimRootMotionTranslationScale = 1.f);

private:
    UPROPERTY()
    UAnimMontage* MontageToPlay;

    // 需要匹配的游戏事件标签容器
    UPROPERTY()
    FGameplayTagContainer EventTags;

    // 播放速率
    UPROPERTY()
    float Rate;

    // 开始播放的段落名称
    UPROPERTY()
    FName StartSection;

    // 根运动平移缩放
    UPROPERTY()
    float AnimRootMotionTranslationScale;

    // 如果为true，则在能力结束时停止蒙太奇
    UPROPERTY()
    bool bStopWhenAbilityEnds;

    // 停止播放蒙太奇
    bool StopPlayingMontage();

    // 蒙太奇开始混合退出时的回调
    void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);
    // 能力被取消时的回调
    void OnAbilityCancelled();
    // 蒙太奇结束时的回调
    void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
    //游戏事件发生时的回调
    void OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload);

    FOnMontageBlendingOutStarted BlendingOutDelegate;
    FOnMontageEnded MontageEndedDelegate;
    FDelegateHandle CancelledHandle;
    FDelegateHandle EventHandle;
};

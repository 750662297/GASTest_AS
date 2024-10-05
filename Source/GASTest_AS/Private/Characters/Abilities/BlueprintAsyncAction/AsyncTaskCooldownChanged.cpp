// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Abilities/BlueprintAsyncAction/AsyncTaskCooldownChanged.h"

#include "ShaderPrintParameters.h"

UAsyncTaskCooldownChanged *UAsyncTaskCooldownChanged::ListenForCooldownChange(UAbilitySystemComponent *AbilitySystemComponent,
                                                                              FGameplayTagContainer    InCooldownTags,
                                                                              bool                     UserServerCooldown)
{
    UAsyncTaskCooldownChanged *ListenForCooldownChange = NewObject<UAsyncTaskCooldownChanged>();
    ListenForCooldownChange->ASC                       = AbilitySystemComponent;
    ListenForCooldownChange->CooldownTags              = InCooldownTags;
    ListenForCooldownChange->UserServerCooldown        = UserServerCooldown;

    if (!IsValid(AbilitySystemComponent) || InCooldownTags.Num() < 1)
    {
        ListenForCooldownChange->EndTask();
        return nullptr;
    }

    AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(
        ListenForCooldownChange, &UAsyncTaskCooldownChanged::OnActiveGameplayEffectAddedCallback);

    TArray<FGameplayTag> CooldownTagArray;
    InCooldownTags.GetGameplayTagArray(CooldownTagArray);

    for (FGameplayTag CooldownTag : CooldownTagArray)
    {
        AbilitySystemComponent->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved)
            .AddUObject(ListenForCooldownChange, &UAsyncTaskCooldownChanged::CooldownTagChanged);
    }

    return ListenForCooldownChange;
}

void UAsyncTaskCooldownChanged::EndTask()
{
    if (IsValid(ASC))
    {
        ASC->OnActiveGameplayEffectAddedDelegateToSelf.RemoveAll(this);

        TArray<FGameplayTag> CooldownTagArray;
        CooldownTags.GetGameplayTagArray(CooldownTagArray);

        for (FGameplayTag CooldownTag : CooldownTagArray)
        {
            ASC->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
        }
    }

    SetReadyToDestroy();
    // 标记对象以便垃圾回收系统处理
    MarkAsGarbage();
}

void UAsyncTaskCooldownChanged::OnActiveGameplayEffectAddedCallback(UAbilitySystemComponent    *Target,
                                                                    const FGameplayEffectSpec  &SpecApplied,
                                                                    FActiveGameplayEffectHandle ActiveHandle)
{
    // 资产标签
    FGameplayTagContainer AssetTags;
    SpecApplied.GetAllAssetTags(AssetTags);

    // 效果标签
    FGameplayTagContainer GrantedTags;
    SpecApplied.GetAllGrantedTags(GrantedTags);

    // 获取所有的冷却标签
    TArray<FGameplayTag> CooldownTagArray;
    CooldownTags.GetGameplayTagArray(CooldownTagArray);

    for (FGameplayTag CooldownTag : CooldownTagArray)
    {
        if (AssetTags.HasTagExact(CooldownTag) || GrantedTags.HasTagExact(CooldownTag))
        {
            float TimeRemaining = 0.0f;
            float Duration      = 0.0f;

            // 假设冷却标签总是第一个标签
            FGameplayTagContainer CooldownTagContainer(GrantedTags.GetByIndex(0));
            GetCooldownRemainingForTag(CooldownTagContainer, TimeRemaining, Duration);

            if (ASC->GetOwnerRole() == ROLE_Authority)
            {
                // Player is Server
                OnCooldownBegin.Broadcast(CooldownTag, TimeRemaining, Duration);
            }
            else if (!UserServerCooldown && SpecApplied.GetContext().GetAbilityInstance_NotReplicated())
            {
                // 客户端使用预测冷却
                OnCooldownBegin.Broadcast(CooldownTag, TimeRemaining, Duration);
            }
            else if (UserServerCooldown && SpecApplied.GetContext().GetAbilityInstance_NotReplicated() == nullptr)
            {
                // 客户端使用服务器的冷却
                OnCooldownBegin.Broadcast(CooldownTag, TimeRemaining, Duration);
            }
            else if (UserServerCooldown && SpecApplied.GetContext().GetAbilityInstance_NotReplicated())
            {
                // 客户端使用服务器的冷却，但这是预测冷却
                OnCooldownBegin.Broadcast(CooldownTag, -1.0f, -1.0f);
            }
        }
    }
}

void UAsyncTaskCooldownChanged::CooldownTagChanged(const FGameplayTag CooldownTag, int32 NewCount)
{
    if (NewCount == 0)
    {
        OnCooldownEnd.Broadcast(CooldownTag, -1.0f, -1.0f);
    }
}

/*
 *查询与给定标签相关的所有活跃的冷却效果，并返回最长的冷却剩余时间和总冷却持续时间
 *
 */
bool UAsyncTaskCooldownChanged::GetCooldownRemainingForTag(FGameplayTagContainer InCooldownTags, float &TimeRemaining,
                                                           float &CooldownDuration)
{
    if (IsValid(ASC) && InCooldownTags.Num() > 0)
    {
        TimeRemaining    = 0.f;
        CooldownDuration = 0.f;

        //创建一个 FGameplayEffectQuery 对象，该对象用于匹配任何拥有 InCooldownTags 中任意一个标签的活跃效果。
        FGameplayEffectQuery const  Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(InCooldownTags);
        //获取所有符合条件的活跃效果的时间信息
        TArray<TPair<float, float>> DurationAndTimeRemaining = ASC->GetActiveEffectsTimeRemainingAndDuration(Query);
        if (DurationAndTimeRemaining.Num() > 0)
        {
            int32 BestIdx = 0;

            float LongestTime = DurationAndTimeRemaining[0].Key;
            for (int32 Idx = 1; Idx < DurationAndTimeRemaining.Num(); ++Idx)
            {
                if (DurationAndTimeRemaining[Idx].Key > LongestTime)
                {
                    LongestTime = DurationAndTimeRemaining[Idx].Key;
                    BestIdx     = Idx;
                }
            }

            TimeRemaining    = DurationAndTimeRemaining[BestIdx].Key;
            CooldownDuration = DurationAndTimeRemaining[BestIdx].Value;

            return true;
        }
    }
    
    return false;
}






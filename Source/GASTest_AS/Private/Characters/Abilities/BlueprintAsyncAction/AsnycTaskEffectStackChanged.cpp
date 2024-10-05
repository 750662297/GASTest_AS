// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Abilities/BlueprintAsyncAction/AsnycTaskEffectStackChanged.h"

UAsnycTaskEffectStackChanged *UAsnycTaskEffectStackChanged::ListenForGameplayEffectStackChange(
    UAbilitySystemComponent *AbilitySystemComponent, FGameplayTag InEffectGameplayTag)
{
    // 先检查有效性
    if (!IsValid(AbilitySystemComponent) || !InEffectGameplayTag.IsValid())
    {
        // 早期退出并返回nullptr，防止创建无效实例
        return nullptr;
    }

    UAsnycTaskEffectStackChanged *ListenForGameplayEffectStackChange = NewObject<UAsnycTaskEffectStackChanged>();
    ListenForGameplayEffectStackChange->ASC                          = AbilitySystemComponent;
    ListenForGameplayEffectStackChange->EffectGameplayTag            = InEffectGameplayTag;

    AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(
        ListenForGameplayEffectStackChange, &UAsnycTaskEffectStackChanged::OnActiveGameplayEffectAddedCallback);
    AbilitySystemComponent->OnAnyGameplayEffectRemovedDelegate().AddUObject(
        ListenForGameplayEffectStackChange, &UAsnycTaskEffectStackChanged::OnRemoveGameplayEffectCallback);

    return ListenForGameplayEffectStackChange;
}

void UAsnycTaskEffectStackChanged::EndTask()
{
    if (IsValid(ASC))
    {
        ASC->OnActiveGameplayEffectAddedDelegateToSelf.RemoveAll(this);
        ASC->OnAnyGameplayEffectRemovedDelegate().RemoveAll(this);

        if (ActiveEffectHandle.IsValid())
        {
            ASC->OnGameplayEffectStackChangeDelegate(ActiveEffectHandle)->RemoveAll(this);
        }
    }

    SetReadyToDestroy();
    MarkAsGarbage();
}

void UAsnycTaskEffectStackChanged::OnActiveGameplayEffectAddedCallback(UAbilitySystemComponent    *Target,
                                                                       const FGameplayEffectSpec  &SpecApplied,
                                                                       FActiveGameplayEffectHandle ActiveHandle)
{
    FGameplayTagContainer AssetTags;
    SpecApplied.GetAllAssetTags(AssetTags);

    FGameplayTagContainer GrantedTags;
    SpecApplied.GetAllGrantedTags(GrantedTags);

    if(AssetTags.HasTagExact(EffectGameplayTag) || GrantedTags.HasTagExact(EffectGameplayTag))
    {
        ASC->OnGameplayEffectStackChangeDelegate(ActiveHandle)->AddUObject(this, &UAsnycTaskEffectStackChanged::GameplayEffectStackChanged);
        OnGameplayEffectStackChanged.Broadcast(EffectGameplayTag, ActiveHandle, 1, 0);
        ActiveEffectHandle = ActiveHandle;
    }
    
}

void UAsnycTaskEffectStackChanged::OnRemoveGameplayEffectCallback(const FActiveGameplayEffect &EffectRemoved)
{
    FGameplayTagContainer AssetTags;
    EffectRemoved.Spec.GetAllAssetTags(AssetTags);

    FGameplayTagContainer GrantedTags;
    EffectRemoved.Spec.GetAllGrantedTags(GrantedTags);

    if(AssetTags.HasTagExact(EffectGameplayTag) || GrantedTags.HasTagExact(EffectGameplayTag))
    {
        OnGameplayEffectStackChanged.Broadcast(EffectGameplayTag,EffectRemoved.Handle, 0,1);
    }
    
}

void UAsnycTaskEffectStackChanged::GameplayEffectStackChanged(FActiveGameplayEffectHandle EffectHandle, int32 NewStackCount,
                                                              int32 PreviousStackCount)
{
    OnGameplayEffectStackChanged.Broadcast(EffectGameplayTag, EffectHandle, NewStackCount,PreviousStackCount);
}




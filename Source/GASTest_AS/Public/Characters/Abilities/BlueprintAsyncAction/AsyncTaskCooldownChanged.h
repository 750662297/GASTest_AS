// Fill out your copyright notice in the Description page of Project Settings.
//侦听 Gameplay Ability冷却时间变化的异步任务类

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "AsyncTaskCooldownChanged.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCooldownChanged, FGameplayTag, CooldownTag, float, TimeRemaining, float, Duration);

/**
 * 
 */

/*ExposedAsyncProxy = AsyncTask 指定这是一个异步任务代理，在蓝图中可以作为异步系欸但使用
 *
 * 
 */
UCLASS(BlueprintType, meta = (ExposedAsyncProxy = AsyncTask))
class GASTEST_AS_API UAsyncTaskCooldownChanged : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnCooldownChanged OnCooldownBegin;

	UPROPERTY(BlueprintAssignable)
	FOnCooldownChanged OnCooldownEnd;


	/*
	 * UserServerCooldown 是否需要考虑服务端的冷却时间
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UAsyncTaskCooldownChanged* ListenForCooldownChange(UAbilitySystemComponent* AbilitySystemComponent, FGameplayTagContainer CooldownTags, bool UserServerCooldown);

    
	UFUNCTION(BlueprintCallable)
	void EndTask();

protected:
	UPROPERTY()
	UAbilitySystemComponent* ASC;

	FGameplayTagContainer CooldownTags;

	bool UserServerCooldown;

	//当新的游戏玩法效果添加到目标组件时被调用
	virtual void OnActiveGameplayEffectAddedCallback(UAbilitySystemComponent* Target, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle);
	//当冷却标签数量发生变化时被调用
	virtual void CooldownTagChanged(const FGameplayTag CooldownTag, int32 NewCount);

	//根据提供的冷却标签获取剩余事件和总冷却时间
	bool GetCooldownRemainingForTag(FGameplayTagContainer InCooldownTags, float& TimeRemaining, float& CooldownDuration);
};

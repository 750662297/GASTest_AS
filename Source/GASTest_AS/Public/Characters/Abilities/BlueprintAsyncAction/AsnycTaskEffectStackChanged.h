// Fill out your copyright notice in the Description page of Project Settings.
// gameplay effect变化时的响应操作
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AbilitySystemComponent.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsnycTaskEffectStackChanged.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnGameplayEffectStackChanged, FGameplayTag, EffectGameplayTag,
											  FActiveGameplayEffectHandle, Handle, int32, NewStackCount, int32,
											  OldStackCount);

/**
 *监听游戏效果在ASC上的堆栈变化
 */
UCLASS(BlueprintType, meta = (ExposedAsyncProxy = AsyncTask))
class GASTEST_AS_API UAsnycTaskEffectStackChanged : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnGameplayEffectStackChanged OnGameplayEffectStackChanged;

	UFUNCTION(BlueprintCallable,meta = (BlueprintInternalUseOnly = "true"))
	static UAsnycTaskEffectStackChanged* ListenForGameplayEffectStackChange(UAbilitySystemComponent* AbilitySystemComponent, FGameplayTag InEffectGameplayTag);

	// You must call this function manually when you want the AsyncTask to end.
	// For UMG Widgets, you would call it in the Widget's Destruct event.
	UFUNCTION(BlueprintCallable)
	void EndTask();

protected:
	UPROPERTY()
	UAbilitySystemComponent* ASC;

	FGameplayTag EffectGameplayTag;

	FActiveGameplayEffectHandle ActiveEffectHandle;

	virtual void OnActiveGameplayEffectAddedCallback(UAbilitySystemComponent* Target, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle);

	virtual void OnRemoveGameplayEffectCallback(const FActiveGameplayEffect& EffectRemoved);

	virtual void GameplayEffectStackChanged(FActiveGameplayEffectHandle EffectHandle, int32 NewStackCount, int32 PreviousStackCount);
};

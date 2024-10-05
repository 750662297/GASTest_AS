// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "GTAT_WaitReceivedDamage.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FWaitReceivedDamageDelegate, class UGTAbilitySystemComponent*, SourceASC,
											   float, UnmitigatedDamage, float, MitigatedDamage);

/**
 *
 */
UCLASS()
class GASTEST_AS_API UGTAT_WaitReceivedDamage : public UAbilityTask
{
	GENERATED_BODY()

	UGTAT_WaitReceivedDamage(const FObjectInitializer &ObjectInitializer);
    
	UPROPERTY(BlueprintAssignable)
	FWaitReceivedDamageDelegate OnDamage;

	virtual void Activate() override;

	UFUNCTION()
	void OnDamageReceived(class UGTAbilitySystemComponent* SourceASC, float UnmitigatedDamage, float MitigatedDamage);

	/*
	 *HidePin
	 *隐藏指定参数的引脚，在蓝图中OwningAbility参数的引脚会被隐藏，这通常用于内部使用的参数，用户不需要直接设置或修改这些参数
	 *DefaultToSelf 为指定参数设置默认值为当前上下文的对象（当蓝图节点生成时，如果 OwningAbility
	 *参数没有显式设置，它将默认为调用该函数的当前对象（通常是拥有该函数的对象）。这在某些情况下可以简化蓝图逻辑，避免不必要的连线）
	 *BlueprintInternalUseOnly
	 *限制该函数只能在内部蓝图逻辑中使用，不能在蓝图的事件图表中直接调用。（这个标签使函数在蓝图编辑器中不可见，但仍然可以通过其他蓝图节点或宏来调用。它主要用于内部实现细节，防止用户在不合适的地方误用该函数）
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks",
			  meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly="TRUE"))
	static UGTAT_WaitReceivedDamage* WaitReceivedDamage(UGameplayAbility* OwningAbility, bool TriggerOnce);

protected:
	bool TriggerOnce;

	virtual void OnDestroy(bool AbilityIsEnding) override;
};

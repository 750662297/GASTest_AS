// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GTAbilitySystemComponent.generated.h"

/*
 *多播委托
 * SourceASC 造成伤害的对象的能力系统组件
 * UnmitigatedDamage 未减免的原始伤害值
 * MitigatedDamage 经过减免后的实际伤害值
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FReceivedDamageDelegate, UGTAbilitySystemComponent* ,SourceASC, float, UnmitigatedDamage, float, MitigatedDamage);

/**
 * 
 */
UCLASS()
class GASTEST_AS_API UGTAbilitySystemComponent : public UAbilitySystemComponent {
	GENERATED_BODY()

public:
	bool bCharacterAbilitiesGiven = false; //角色是否已经初始化其能力
	bool bStartupEffectsApplied = false; //标记在游戏开始时是否已经给角色应用了启动时的效果（初始默认效果）

	FReceivedDamageDelegate ReceivedDamage;

	//受到伤害时调用，通知所有已绑定的监听者有关伤害的信息
	virtual void ReceiveDamage(UGTAbilitySystemComponent* SourceASC, float UnmitigatedDamage, float MitigatedDamage);
};

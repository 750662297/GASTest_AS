// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GASTest_AS/GASTest_AS.h"
#include "GTGameplayAbility.generated.h"

/**
 *
 */
UCLASS()
class GASTEST_AS_API UGTGameplayAbility : public UGameplayAbility {
	GENERATED_BODY()

public:
	UGTGameplayAbility();

	//当按下输入键时，此设置的技能将自动激活
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	EGTAbilityInputID AbilityInputID = EGTAbilityInputID::None;

	//将技能与插槽关联起来，而不将其绑定到自动激活的输入
	//被动技能不会与输入绑定，所以我们需要一种方法将技能与插槽关联起来
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	EGTAbilityInputID AbilityID = EGTAbilityInputID::None;

	//指示该技能是否在玩家获得时立刻激活（被动技能不需要等待任何输入激活）
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	bool ActivateAbilityOnGranted = false;

	//当能力被赋予一个角色时会被调用，执行一些初始化逻辑（立即激活被动技能等）
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilitySpec &Spec) override;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	FText AbilityName;

protected:
	UFUNCTION()
	void StartCooldown(float CooldownDuration, FGameplayTag AbilityTag);

	UFUNCTION()
	virtual void OnCooldownEnd(FGameplayTag AbilityTag);
};

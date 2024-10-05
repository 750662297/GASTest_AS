// Fill out your copyright notice in the Description page of Project Settings.
// gameplay Ability属性

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GTAttributeSetBase.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class GASTEST_AS_API UGTAttributeSetBase : public UAttributeSet
{
	GENERATED_BODY()

public:
	UGTAttributeSetBase();

	/*
	 *在某个属性值改变之前调用，在属性值实际更新之前做一些准备工作或逻辑
	 * Attribute：一个FGameplayAttribute类型的引用，表示即将发生变化的属性。
	 * NewValue：一个浮点数类型的引用，表示即将设置的新值。你可以修改这个值，从而改变最终赋给属性的实际值。
	 *
	 * 场景：验证新值是否超出合理的范围；在属性值改变前执行一些前置逻辑，例如扣除资源、检查条件等。
	 */
	
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	/*
	 *在游戏效果（effect）执行完毕后被调用，用于处理游戏效果之后的逻辑，比如更新UI，播放动画，触发事件等
	 * Data：一个FGameplayEffectModCallbackData类型的常量引用，包含了游戏效果执行的相关信息，包括被影响的属性、调整量等。
	* 场景：更新UI显示，比如在角色受到伤害后更新生命值UI；触发某些事件或动作，例如在角色获得增益效果后播放特定音效。
	 */
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	/*
	 *用于指定哪些属性需要在网络间复制（在多人游戏中，为了让客户端和服务器端的数据保持一致，需要明确哪些属性需要在网络之间同步）
	 * OutLifetimeProps：一个TArray<FLifetimeProperty>类型的引用，用来存储需要复制的属性列表。
	* 场景：确保重要的游戏数据在网络间同步，如角色的位置、生命值、经验值等；优化网络流量，只复制必要的数据。
	 */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//属性

	//ReplicatedUsing = OnRep_Health 指定当这个属性发生改变并需要在网络间复制时，应该调用OnRep_Health函数
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UGTAttributeSetBase,Health)

	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UGTAttributeSetBase,MaxHealth)

	//每秒恢复的生命值
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_HealthRegenRate)
	FGameplayAttributeData HealthRegenRate;
	ATTRIBUTE_ACCESSORS(UGTAttributeSetBase,HealthRegenRate)

	UPROPERTY(BlueprintReadOnly, Category = "Armor", ReplicatedUsing = OnRep_Armor)
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(UGTAttributeSetBase,Armor)

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UGTAttributeSetBase, Damage)

	UPROPERTY(BlueprintReadOnly, Category = "Mana", ReplicatedUsing = OnRep_Mana)
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UGTAttributeSetBase,Mana)

	UPROPERTY(BlueprintReadOnly, Category = "Mana", ReplicatedUsing = OnRep_MaxMana)
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UGTAttributeSetBase,MaxMana)
    UPROPERTY(BlueprintReadOnly, Category = "Mana", ReplicatedUsing = OnRep_ManaRegenRate)
	FGameplayAttributeData ManaRegenRate;
	ATTRIBUTE_ACCESSORS(UGTAttributeSetBase,ManaRegenRate)

	UPROPERTY(BlueprintReadOnly, Category = "MoveSpeed", ReplicatedUsing = OnRep_MoveSpeed)
	FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(UGTAttributeSetBase,MoveSpeed)

	UPROPERTY(BlueprintReadOnly, Category = "Character Level", ReplicatedUsing = OnRep_CharacterLevel)
	FGameplayAttributeData CharacterLevel;
	ATTRIBUTE_ACCESSORS(UGTAttributeSetBase,CharacterLevel)

	UPROPERTY(BlueprintReadOnly, Category = "XP", ReplicatedUsing = OnRep_XP)
	FGameplayAttributeData XP;
	ATTRIBUTE_ACCESSORS(UGTAttributeSetBase,XP)

	//经验值奖励
	UPROPERTY(BlueprintReadOnly, Category = "XP", ReplicatedUsing = OnRep_XPBounty)
	FGameplayAttributeData XPBounty;
	ATTRIBUTE_ACCESSORS(UGTAttributeSetBase,XPBounty)

	UPROPERTY(BlueprintReadOnly, Category = "Gold", ReplicatedUsing = OnRep_Gold)
	FGameplayAttributeData Gold;
	ATTRIBUTE_ACCESSORS(UGTAttributeSetBase,Gold)

	UPROPERTY(BlueprintReadOnly, Category = "Gold", ReplicatedUsing = OnRep_GoldBounty)
	FGameplayAttributeData GoldBounty;
	ATTRIBUTE_ACCESSORS(UGTAttributeSetBase,GoldBounty)

protected:
	void AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty);
	
	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	UFUNCTION()
	virtual void OnRep_HealthRegenRate(const FGameplayAttributeData& OldHealthRegenRate);

	UFUNCTION()
	virtual void OnRep_Armor(const FGameplayAttributeData& OldArmor);

	UFUNCTION()
	virtual void OnRep_Mana(const FGameplayAttributeData& OldMana);
	
	UFUNCTION()
	virtual void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana);
    UFUNCTION()
	virtual void OnRep_ManaRegenRate(const FGameplayAttributeData& OldManaRegenRate);
	
	UFUNCTION()
	virtual void OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed);

	UFUNCTION()
	virtual void OnRep_CharacterLevel(const FGameplayAttributeData& OldCharacterLevel);

	UFUNCTION()
	virtual void OnRep_XP(const FGameplayAttributeData& OldXP);

	UFUNCTION()
	virtual void OnRep_XPBounty(const FGameplayAttributeData& OldXPBounty);

	UFUNCTION()
	virtual void OnRep_Gold(const FGameplayAttributeData& OldGold);

	UFUNCTION()
	virtual void OnRep_GoldBounty(const FGameplayAttributeData& OldGoldBounty);
	
private:
	FGameplayTag HitDirectionFrontTag;
	FGameplayTag HitDirectionBackTag;
	FGameplayTag HitDirectionRightTag;
	FGameplayTag HitDirectionLeftTag;
};

#pragma once

#include "AbilitySystemInterface.h"
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerState.h"
#include "GameplayEffectTypes.h"
#include "GTPlayerState.generated.h"

/**
 *
 */
UCLASS()
class GASTEST_AS_API AGTPlayerState : public APlayerState, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    AGTPlayerState();

    class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

    class UGTAttributeSetBase* GetAttributeSetBase() const;

    UFUNCTION(BlueprintCallable, Category = "GASTest|GTPlayerState")
    bool IsAlive() const;

    UFUNCTION(BlueprintCallable, Category = "GASTest|GTPlayerState")
    void ShowAbilityConfrimCancelText(bool ShowText);

    /*
     *Getters for attributes from GTAttributeSetBase, Returns Current Value unless otherwise specified
     *
     */
    UFUNCTION(BlueprintCallable, Category = "GASTest|GTPlayerState|Attributes")
    float GetHealth() const;

    UFUNCTION(BlueprintCallable, Category = "GASTest|GTPlayerState|Attributes")
    float GetMaxHealth() const;

    UFUNCTION(BlueprintCallable, Category = "GASTest|GTPlayerState|Attributes")
    float GetHealthRegenRate() const;

    UFUNCTION(BlueprintCallable, Category = "GASTest|GTPlayerState|Attributes")
    float GetMana() const;

    UFUNCTION(BlueprintCallable, Category = "GASTest|GTPlayerState|Attributes")
    float GetMaxMana() const;

    UFUNCTION(BlueprintCallable, Category = "GASTest|GTPlayerState|Attributes")
    float GetManaRegenRate() const;

    UFUNCTION(BlueprintCallable, Category = "GASTest|GTPlayerState|Attributes")
    float GetStamina() const;

    UFUNCTION(BlueprintCallable, Category = "GASTest|GTPlayerState|Attributes")
    float GetMaxStamina() const;

    UFUNCTION(BlueprintCallable, Category = "GASTest|GTPlayerState|Attributes")
    float GetStaminaRegenRate() const;

    UFUNCTION(BlueprintCallable, Category = "GASTest|GTPlayerState|Attributes")
    float GetArmor() const;

    UFUNCTION(BlueprintCallable, Category = "GASTest|GTPlayerState|Attributes")
    float GetMoveSpeed() const;

    UFUNCTION(BlueprintCallable, Category = "GASTest|GTPlayerState|Attributes")
    int32 GetCharacterLevel() const;

    UFUNCTION(BlueprintCallable, Category = "GASTest|GTPlayerState|Attributes")
    int32 GetXP() const;

    UFUNCTION(BlueprintCallable, Category = "GASTest|GTPlayerState|Attributes")
    int32 GetXPBounty() const;

    UFUNCTION(BlueprintCallable, Category = "GASTest|GTPlayerState|Attributes")
    int32 GetGold() const;

    UFUNCTION(BlueprintCallable, Category = "GASTest|GTPlayerState|Attributes")
    int32 GetGoldBounty() const;

protected:
    UPROPERTY()
    class UGTAbilitySystemComponent* AbilitySystemComponent;

    UPROPERTY()
    class UGTAttributeSetBase* AttributeSetBase;

    FGameplayTag DeadTag;

    FDelegateHandle HealthChangedDelegateHandle;
    FDelegateHandle MaxHealthChangedDelegateHandle;
    FDelegateHandle HealthRegenRateChangedDelegateHandle;
    FDelegateHandle ManaChangedDelegateHandle;
    FDelegateHandle MaxManaChangedDelegateHandle;
    FDelegateHandle ManaRegenRateChangedDelegateHandle;
    // FDelegateHandle StaminaChangedDelegateHandle;
    // FDelegateHandle MaxStaminaChangedDelegateHandle;
    // FDelegateHandle StaminaRegenRateChangedDelegateHandle;
    FDelegateHandle XPChangedDelegateHandle;
    FDelegateHandle GoldChangedDelegateHandle;
    FDelegateHandle CharacterLevelChangedDelegateHandle;

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    // Attribute changed callbacks
    virtual void HealthChanged(const FOnAttributeChangeData& Data);
    virtual void MaxHealthChanged(const FOnAttributeChangeData& Data);
    virtual void HealthRegenRateChanged(const FOnAttributeChangeData& Data);
    virtual void ManaChanged(const FOnAttributeChangeData& Data);
    virtual void MaxManaChanged(const FOnAttributeChangeData& Data);
    virtual void ManaRegenRateChanged(const FOnAttributeChangeData& Data);
    // virtual void StaminaChanged(const FOnAttributeChangeData& Data);
    // virtual void MaxStaminaChanged(const FOnAttributeChangeData& Data);
    // virtual void StaminaRegenRateChanged(const FOnAttributeChangeData& Data);
    virtual void XPChanged(const FOnAttributeChangeData& Data);
    virtual void GoldChanged(const FOnAttributeChangeData& Data);
    virtual void CharacterLevelChanged(const FOnAttributeChangeData& Data);

    // FGameplayTag状态发生变更时回调
    virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
};

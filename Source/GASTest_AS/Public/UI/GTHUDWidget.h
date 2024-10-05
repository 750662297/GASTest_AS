
#pragma once

#include "CoreMinimal.h"
#include "GameplayTags.h"
#include "Blueprint/UserWidget.h"
#include "Components/ListView.h"
#include "GTCooldownItemWidget.h"
#include "GTHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class GASTEST_AS_API UGTHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ShowAbilityConfirmCancelText(bool ShowText);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetRespawnCountdown(float RespawnTimeRemaining);

	UFUNCTION(BlueprintCallable)
	void AddCooldownInfo(const FText& AbilityName, FGameplayTag AbilityTag, float CooldownDuration,
	                     float currentTime = 0);
	
	UFUNCTION(BlueprintCallable)
	void RemoveCooldown(FGameplayTag AbilityTag);

	UFUNCTION(BlueprintCallable)
	void UpdateBufferNumber(FGameplayTag AbilityTag, int32 Number);
	
	/**
	* Attribute setters
	*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetMaxHealth(float MaxHealth);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetCurrentHealth(float CurrentHealth);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetHealthPercentage(float HealthPercentage);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetHealthRegenRate(float HealthRegenRate);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetMaxStamina(float MaxStamina);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetCurrentStamina(float CurrentStamina);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetStaminaPercentage(float StaminaPercentage);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetStaminaRegenRate(float StaminaRegenRate);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetMaxMana(float MaxMana);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetCurrentMana(float CurrentMana);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetManaPercentage(float ManaPercentage);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetManaRegenRate(float ManaRegenRate);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetExperience(int32 Experience);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetHeroLevel(int32 HeroLevel);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetGold(int32 Gold);

protected:
	UFUNCTION(BlueprintCallable)
	void UpdateCooldownInfo(const FText& AbilityName, FGameplayTag AbilityTag, float CooldownDuration,
	                          float CurrentTime);

	UFUNCTION(BlueprintCallable)
	void UpdateCooldownItems(TArray<FGameplayTag> ItemsToUpdate);

	UFUNCTION(BlueprintCallable)
	void RemoveCooldownItems(TArray<FGameplayTag> ItemsToRemove);
	
	UFUNCTION(BlueprintCallable)
	void AddCooldownItem(const FText& Name, FGameplayTag AbilityTag, float Duration, float Current);

	
	
	UFUNCTION()
	void OnTimerout();
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HUD|Timer")
	float TimerInvertal=0.1f;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UListView* ListView;
	
private:
	UPROPERTY()
	TMap<FGameplayTag, UGTCooldownItemData*> CooldownAbilities;

	UPROPERTY()
	FTimerHandle HUDTimerHandle;
};

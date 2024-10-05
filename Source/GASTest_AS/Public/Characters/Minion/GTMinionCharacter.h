// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/GTCharacterBase.h"
#include "GameplayEffectTypes.h"
#include "GTMinionCharacter.generated.h"

UCLASS()
class GASTEST_AS_API AGTMinionCharacter : public AGTCharacterBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGTMinionCharacter(const class FObjectInitializer& ObjectInitializer);

protected:

	UPROPERTY()
	class UGTAbilitySystemComponent* HardRefAbilitySystemComponent;

	UPROPERTY()
	class UGTAttributeSetBase* HardRefAttributeSetBase;
    
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="GASTest|UI")
	TSubclassOf<class UGTFloatingStatusBarWidget> UIFloatingStatusBarClass;

	UPROPERTY()
	class UGTFloatingStatusBarWidget* UIFloatingStatusBar;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "GASTest|UI")
	class UWidgetComponent* UIFloatingStatusBarComponent;

	FDelegateHandle HealthChangedDelegateHandle;
    
	virtual void HealthChanged(const FOnAttributeChangeData& Data);

	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
};

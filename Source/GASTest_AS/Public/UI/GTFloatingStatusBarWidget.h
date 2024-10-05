// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GTFloatingStatusBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class GASTEST_AS_API UGTFloatingStatusBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/*
	 *BlueprintImplementableEvent 示这是一个可以由蓝图实现的事件。这意味着在蓝图中可以覆盖这些函数并实现具体的逻辑。
	 * BlueprintCallable 表示可以在蓝图中直接调用
	 */
    
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetHealthPercentage(float HealthPercentage);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetManaPercentage(float ManaPercentage);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetCharacterName(const FText &NewName);
};

// Fill out your copyright notice in the Description page of Project Settings.
// 侦听并响应角色或对象的属性变化
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AbilitySystemComponent.h"
#include "AsyncTaskAttributeChanged.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAttributeChanged, FGameplayAttribute, Attribute, float, NewValue, float ,OldValue);

/**
 * 
 */
UCLASS(BlueprintType, meta = (ExposedAsyncProxy = AsyncTask))
class GASTEST_AS_API UAsyncTaskAttributeChanged : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChanged OnAttributeChanged;

	//Listens for an attribute changing.
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UAsyncTaskAttributeChanged* ListenForAttributeChange(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAttribute Attribute);

	// Listens for an attribute changing.
	// Version that takes in an array of Attributes. Check the Attribute output for which Attribute changed.
	UFUNCTION(BlueprintCallable,meta = (BlueprintInternalUseOnly = "true"))
	static UAsyncTaskAttributeChanged* ListenForAttributesChange(UAbilitySystemComponent* AbilitySystemComponent, TArray<FGameplayAttribute> Attributes);

	UFUNCTION(BlueprintCallable)
	void EndTask();

protected:
	UPROPERTY()
	UAbilitySystemComponent* ASC;

	FGameplayAttribute AttributeToListenFor;
	TArray<FGameplayAttribute> AttributesToListenFor;

	void AttributeChanged(const FOnAttributeChangeData& Data);
};

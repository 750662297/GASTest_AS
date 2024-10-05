// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTags.h"
#include "GTCooldownItemData.generated.h"

/**
 * 
 */
UCLASS()
class GASTEST_AS_API UGTCooldownItemData : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ListItem")
	FText AbilityName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ListItem")
	FGameplayTag AbilityTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ListItem")
	float Duration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ListItem")
	float Current;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ListItem")
	int32 BuffNumber=0; //技能层数
};


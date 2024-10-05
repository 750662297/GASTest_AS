// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "GTAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class GASTEST_AS_API UGTAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	static UGTAssetManager& Get();

	virtual void StartInitialLoading() override;
};

// Fill out your copyright notice in the Description page of Project Settings.

#include "GTAssetManager.h"
#include "AbilitySystemGlobals.h"

UGTAssetManager &UGTAssetManager::Get()
{
	UGTAssetManager* Singleton = Cast<UGTAssetManager>(GEngine->AssetManager);
	if(Singleton)
	{
		return *Singleton;
	}
	else
	{
		UE_LOG(LogTemp, Fatal, TEXT("Invalid AssetManager in DefaultEngine.ini, must be GDAssetManager!"));
		return *NewObject<UGTAssetManager>();	 // never calls this
	}
}

void UGTAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	UAbilitySystemGlobals::Get().InitGlobalData();
}

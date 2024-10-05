// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GASTestGameMode.generated.h"

UCLASS(minimalapi)
class AGASTestGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGASTestGameMode();

	void HeroDied(AController* Controller);

protected:
	//复活时间延迟
	float RespawnDelay;
	//英雄角色子类
	TSubclassOf<class AGTHeroCharacter>  HeroClass;
	//敌人生成点
	AActor* EnemySpawnPoint;

	virtual void BeginPlay() override;
	//复活英雄
	void RespawnHero(AController* Controller);
};




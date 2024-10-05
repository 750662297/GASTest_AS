// Copyright Epic Games, Inc. All Rights Reserved.

#include "GASTestGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/SpectatorPawn.h"
#include "Characters/Hero/GTHeroCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Player/GTPlayerController.h"

AGASTestGameMode::AGASTestGameMode()
{
    RespawnDelay = 5.0f;

    HeroClass = StaticLoadClass(UObject::StaticClass(), nullptr,
                                TEXT("/Game/GASTest/Characters/Hero/BP_HeroCharacter.BP_HeroCharacter_C"));
    if (!HeroClass)
    {
        UE_LOG(LogTemp, Error,
               TEXT("%s() Failed to find HeroClass. If it was moved, please update the reference location in C++."),
               *FString(__FUNCTION__));
    }
}

void AGASTestGameMode::HeroDied(AController *Controller)
{
    /*
     *生成新Actor时处理可能的碰撞情况
     * SpawnCollisionHandlingOverride 是FActorSpawnParameters中的一个枚举类型成员变量，指定生成新Actor时如何处理碰撞
     * AlwaysSpawn 总是生成，忽略碰撞
     * DontSpawnIfColliding 如果碰撞则不生成
     * AdjustIfPossibleButAlwaysSpawn 如果可能的话调整位置以避免碰撞，但如果无法调整则仍然生成。
     * AdjustIfPossibleButDontSpawnIfStillColliding 如果可能的话调整位置以免碰撞，但无法调整则不生成
     */
    FActorSpawnParameters SpawnParameters;
    SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    //创建一个观察者Pawn，并设置其初始位置和方向
    ASpectatorPawn *SpectatorPawn = GetWorld()->SpawnActor<ASpectatorPawn>(
        SpectatorClass, Controller->GetPawn()->GetActorTransform(), SpawnParameters);

    //让控制器脱离当前Pawn，并附加到新的观察者Pawn上
    Controller->UnPossess();
    Controller->Possess(SpectatorPawn);

    //初始化复活计时器
    FTimerHandle   RespawnTimerHandle;
    FTimerDelegate RespawnDelegate;

    //设置复活委托，当计时结束时调用 RespawnHero函数
    RespawnDelegate = FTimerDelegate::CreateUObject(this, &AGASTestGameMode::RespawnHero, Controller);

    /*设置计时器
     * RespawnTimerHandle 句柄（标识符），唯一标识这个定时器
     * RespawnDelegate 定时器到期时执行的委托
     * RespawnDelay 延迟时间（秒）
     * InbLoop 是否重复触发（当前设置为false）
     */
    GetWorldTimerManager().SetTimer(RespawnTimerHandle, RespawnDelegate, RespawnDelay, false);

    AGTPlayerController *PC = Cast<AGTPlayerController>(Controller);
    if (PC)
    {
        PC->SetRespawnCountdown(RespawnDelay);
    }
}

void AGASTestGameMode::BeginPlay()
{
    Super::BeginPlay();

    //获取敌人生成位置
    TArray<AActor *> Actors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), Actors);
    for (AActor *Actor : Actors)
    {
        if (Actor->GetName() == FString("EnemyHeroSpawn"))
        {
            EnemySpawnPoint = Actor;
            break;
        }
    }
}

void AGASTestGameMode::RespawnHero(AController *Controller)
{
    if (Controller->IsPlayerController())
    {
        //Respawn player hero
        //查找玩家起点
        AActor* PlayerStart = FindPlayerStart(Controller);

        //设置重生参数
        FActorSpawnParameters SpawnParameters;
        SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        //生成英雄角色
        AGTHeroCharacter* Hero = GetWorld()->SpawnActor<AGTHeroCharacter>(HeroClass, PlayerStart->GetActorLocation(), PlayerStart->GetActorRotation(),SpawnParameters);

        //解除旧角色控制并销毁，让控制器控制新生成的角色
        APawn* OldSpectatorPawn = Controller->GetPawn();
        Controller->UnPossess();
        OldSpectatorPawn->Destroy();
        Controller->Possess(Hero);
    }

    else
    {
        //Respawn AI hero

        FActorSpawnParameters SpawnParameters;
        SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        AGTHeroCharacter* Hero = GetWorld()->SpawnActor<AGTHeroCharacter>(HeroClass, EnemySpawnPoint->GetActorTransform(),SpawnParameters);

        APawn* OldSpectatorPawn = Controller->GetPawn();
        Controller->UnPossess();
        OldSpectatorPawn->Destroy();
        Controller->Possess(Hero);
    }
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/GTCharacterBase.h"
#include "GameFramework/Character.h"
#include "GTHeroCharacter.generated.h"

UCLASS()
class GASTEST_AS_API AGTHeroCharacter : public AGTCharacterBase
{
    GENERATED_BODY()

public:
    // Sets default values for this character's properties
    AGTHeroCharacter(const class FObjectInitializer& ObjectInitializer);

    virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;

    //只在服务器被调用，用于处理控制器的占用
    virtual void PossessedBy(AController *NewController) override;

    class USpringArmComponent* GetCameraBoom();

    class UCameraComponent* GetFollowCamera();

    UFUNCTION(BlueprintCallable, Category = "GASTest|Camera")
    float GetStartingCameraBoomArmLength();

    UFUNCTION(BlueprintCallable, Category = "GASTest|Camera")
    FVector GetStartingCameraBoomLocaltion();

    class UGTFloatingStatusBarWidget* GetFloatingStatusBar();

    USkeletalMeshComponent* GetGunComponent() const;

    virtual void FinishDying() override;
    
protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    //在Actor的所有组件都已初始化之后执行某些操作
    virtual void PostInitializeComponents() override;

    void LookUp(float Value);

    void LookUpRate(float Value);

    void Turn(float Value);

    void TurnRate(float Value);

    void MoveForward(float Value);

    void MoveRight(float Value);

    //创建并初始化英雄的浮动状态栏
    //多次调用是安全的，通过检查保证只执行一次
    UFUNCTION()
    void InitializeFloatingStatusBar();

    //用于在检测到玩家状态发生变化时执行某些操作
    virtual void OnRep_PlayerState() override;

    // Called from both SetupPlayerInputComponent and OnRep_PlayerState because of a potential race condition where the PlayerController might
    // call ClientRestart which calls SetupPlayerInputComponent before the PlayerState is repped to the client so the PlayerState would be null in SetupPlayerInputComponent.
    // Conversely, the PlayerState might be repped before the PlayerController calls ClientRestart so the Actor's InputComponent would be null in OnRep_PlayerState.
    void BindASCInput();
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GASTest|Camera")
    float BaseTurnRate = 45.0f;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GASTest|Camera")
    float BaseLookUpRate = 45.0f;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASTest|Camera")
    float StartingCameraBoomArmLength;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASTest|Camera")
    FVector StartingCameraBoomLocation;


    UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "GASTest|Camera")
    class USpringArmComponent* CameraBoom;

    UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "GASTest|Camera")
    class UCameraComponent* FollowCamera;

    UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
    USkeletalMeshComponent* GunComponent;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GASTest|UI")
    TSubclassOf<class UGTFloatingStatusBarWidget> UIFloatingStatusBarClass;

    UPROPERTY()
    class UGTFloatingStatusBarWidget* UIFloatingStatusBar;

    UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "GASTest|UI")
    class UWidgetComponent* UIFloatingStatusBarComponent;

    bool ASCInputBound =false;

    FGameplayTag DeadTag;

};

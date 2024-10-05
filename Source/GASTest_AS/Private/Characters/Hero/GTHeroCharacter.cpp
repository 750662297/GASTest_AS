// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Hero/GTHeroCharacter.h"

#include "AI/GTHeroAIController.h"
#include "Camera/CameraComponent.h"
#include "Characters/Abilities/GTAbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GASTestGameMode.h"
#include "Player/GTPlayerState.h"
#include "UI/GTFloatingStatusBarWidget.h"
#include "Characters/Abilities/AttributeSets/GTAttributeSetBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/GTPlayerController.h"

// Sets default values
AGTHeroCharacter::AGTHeroCharacter(const class FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(FName("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->bUsePawnControlRotation = true;
    CameraBoom->SetRelativeLocation(FVector(0, 0, 68.492264));

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(FName("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom);
    FollowCamera->FieldOfView = 80.0f;

    GunComponent = CreateDefaultSubobject<USkeletalMeshComponent>(FName("Gun"));

    //设置胶囊提碰撞组件行为，特别是对于“相机”类型的碰撞通道设置为忽略响应
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

    // Makes sure that the animations play on the Server so that we can use bone and socket transforms
    // to do things like spawning projectiles and other FX.
    // 配置了角色主体网格的动画更新选项，确保即使在网络中也能正确播放动画；同时关闭了网格上的物理碰撞检测

    //不管骨骼网格是否可见，都会始终更新其姿态并刷新骨骼（即使角色不在屏幕上显示，它的动画也会继续播放，并且骨骼的位置和旋转会保持最新）
    //以确保服务器上的动画总是更新，以便可以使用骨骼位置来生成子弹或其他效果。
    //保证在网络游戏中，客户端和服务器之间的动画同步更加准确
    GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision); //禁用碰撞检测
    GetMesh()->SetCollisionProfileName(FName("NoCollision"));       //设置碰撞配置文件

    UIFloatingStatusBarComponent = CreateDefaultSubobject<UWidgetComponent>(FName("UIFloatingStatusBarComponent"));
    UIFloatingStatusBarComponent->SetupAttachment(RootComponent);
    UIFloatingStatusBarComponent->SetRelativeLocation(FVector(0, 0, 120));
    UIFloatingStatusBarComponent->SetWidgetSpace(EWidgetSpace::Screen);
    UIFloatingStatusBarComponent->SetDrawSize(FVector2D(500, 500));

    UIFloatingStatusBarClass = StaticLoadClass(UObject::StaticClass(), nullptr,
                                               TEXT(
                                                   "/Game/GASTest/UI/UI_FloatingStatusBar_Hero.UI_FloatingStatusBar_Hero_C"));
    if (!UIFloatingStatusBarClass)
    {
        UE_LOG(LogTemp, Error,
               TEXT(
                   "%s() Failed to find UIFloatingStatusBarClass. If it was moved, please update the reference location in C++."
               ), *FString(__FUNCTION__));
    }

    AIControllerClass = AGTHeroAIController::StaticClass();

    DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
}

// Called to bind functionality to input
void AGTHeroCharacter::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &AGTHeroCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AGTHeroCharacter::MoveRight);

    PlayerInputComponent->BindAxis("LookUp", this, &AGTHeroCharacter::LookUp);
    PlayerInputComponent->BindAxis("LookUpRate", this, &AGTHeroCharacter::LookUpRate);
    PlayerInputComponent->BindAxis("Turn", this, &AGTHeroCharacter::Turn);
    PlayerInputComponent->BindAxis("TurnRate", this, &AGTHeroCharacter::TurnRate);

    // Bind player input to the AbilitySystemComponent. Also called in OnRep_PlayerState because of a potential race condition.
    BindASCInput();
}

// Server only
void AGTHeroCharacter::PossessedBy(AController *NewController)
{
    Super::PossessedBy(NewController);

    AGTPlayerState *PS = GetPlayerState<AGTPlayerState>();
    if (PS == nullptr)
    {
        return;
    }

    AbilitySystemComponent = Cast<UGTAbilitySystemComponent>(PS->GetAbilitySystemComponent());
    PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);

    AttributeSetBase = PS->GetAttributeSetBase();

    InitializeAttributes();

    //将DeadTag的计数强制设置为0
    AbilitySystemComponent->SetTagMapCount(DeadTag, 0);

    SetHealth(GetMaxHealth());
    SetMana(GetMaxMana());
    SetStamia(GetMaxStamina());

    AddStartupEffects();
    AddCharacterAbilities();

    AGTPlayerController *PC = Cast<AGTPlayerController>(GetController());
    if (PC)
    {
        PC->CreateHUD();
    }

    InitializeFloatingStatusBar();
}

USpringArmComponent *AGTHeroCharacter::GetCameraBoom()
{
    return CameraBoom;
}

UCameraComponent *AGTHeroCharacter::GetFollowCamera()
{
    return FollowCamera;
}

float AGTHeroCharacter::GetStartingCameraBoomArmLength()
{
    return StartingCameraBoomArmLength;
}

FVector AGTHeroCharacter::GetStartingCameraBoomLocaltion()
{
    return StartingCameraBoomLocation;
}

UGTFloatingStatusBarWidget *AGTHeroCharacter::GetFloatingStatusBar()
{
    return UIFloatingStatusBar;
}

USkeletalMeshComponent *AGTHeroCharacter::GetGunComponent() const
{
    return GunComponent;
}

void AGTHeroCharacter::FinishDying()
{
    if (GetLocalRole() == ROLE_Authority)
    {
        AGASTestGameMode *GM = Cast<AGASTestGameMode>(GetWorld()->GetAuthGameMode());

        if (GM)
        {
            GM->HeroDied(GetController());
        }
    }

    Super::FinishDying();
}


/*
 *On the Server , Possession happens before BeginPlay
 *On the Client, BeginPlay happens before Possession
 *So we can't use BeginPlay to do anything with the AbilitySystemComponent because we don't have it until the PlayerState replicates from possession.
 */
void AGTHeroCharacter::BeginPlay()
{
    Super::BeginPlay();

    InitializeFloatingStatusBar();
    StartingCameraBoomArmLength = CameraBoom->TargetArmLength;
    StartingCameraBoomLocation  = CameraBoom->GetRelativeLocation();
}

void AGTHeroCharacter::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    if (GunComponent && GetMesh())
    {
        //SnapToTargetNotIncludingScale 指定位置和旋转会调整为与目标保持一致，但缩放保持不变
        GunComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
                                        FName("GunSocket"));
    }
}

void AGTHeroCharacter::LookUp(float Value)
{
    if (IsAlive())
    {
        AddControllerPitchInput(Value);
    }
}

void AGTHeroCharacter::LookUpRate(float Value)
{
    if (IsAlive())
    {
        AddControllerPitchInput(Value * BaseLookUpRate * GetWorld()->DeltaTimeSeconds);
    }
}

void AGTHeroCharacter::Turn(float Value)
{
    if (IsAlive())
    {
        AddControllerYawInput(Value);
    }
}

void AGTHeroCharacter::TurnRate(float Value)
{
    if (IsAlive())
    {
        AddControllerYawInput(Value * BaseTurnRate * GetWorld()->DeltaTimeSeconds);
    }
}

void AGTHeroCharacter::MoveForward(float Value)
{
    /*
     *UKismetMathLibrary::GetForwardVector 计算一个方向向量（在这里用来获取角色面向的方向）
     *GetControlRotation 获取玩家水平面的旋转（忽略俯仰Pitch，偏航Yaw）
     * 
     */
    FVector Vector = UKismetMathLibrary::GetForwardVector(FRotator(0, GetControlRotation().Yaw, 0));
    AddMovementInput(Vector, Value);
}

void AGTHeroCharacter::MoveRight(float Value)
{
    AddMovementInput(UKismetMathLibrary::GetRightVector(FRotator(0, GetControlRotation().Yaw, 0)), Value);
}

void AGTHeroCharacter::InitializeFloatingStatusBar()
{
    // only create once
    if (UIFloatingStatusBar || !AbilitySystemComponent.IsValid())
    {
        return;
    }

    //Setup UI for locally Owned Players only, not AI or the server's copy of the PlayerControllers

    /*
     *不直接从GetController转换的原因
     * GetController() 是 APawn 类的一个成员函数，它返回当前控制此 Pawn 的控制器。然而，在某些情况下，尤其是服务器端代码执行时，这个控制器可能并不是本地玩家控制器。
     * UGameplayStatics::GetPlayerController(GetWorld(), 0) 会返回本地玩家的控制器，这是因为它通过世界对象（UWorld）来查找第一个玩家控制器，而这个控制器通常就是本地玩家控制器。
     */
    AGTPlayerController *PC = Cast<AGTPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
    if (PC && PC->IsLocalPlayerController())
    {
        if (UIFloatingStatusBarClass)
        {
            UIFloatingStatusBar = CreateWidget<UGTFloatingStatusBarWidget>(PC, UIFloatingStatusBarClass);
            if (UIFloatingStatusBar && UIFloatingStatusBarComponent)
            {
                UIFloatingStatusBarComponent->SetWidget(UIFloatingStatusBar);

                //Setup the floating status Bar
                UIFloatingStatusBar->SetHealthPercentage(GetHealth() / GetMaxHealth());
                UIFloatingStatusBar->SetManaPercentage(GetMana() / GetMaxMana());
            }
        }
    }
}

//Client only
void AGTHeroCharacter::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();

    AGTPlayerState *PS = GetPlayerState<AGTPlayerState>();
    if (PS == nullptr)
    {
        return;
    }

    //Set the ASC for clients, Server does this PossessedBy
    AbilitySystemComponent = Cast<UGTAbilitySystemComponent>(PS->GetAbilitySystemComponent());

    //Init ASC Actor Info for clients, Server will init its ASC when it possesses a new Actor
    AbilitySystemComponent->InitAbilityActorInfo(PS, this);

    // Bind player input to the AbilitySystemComponent. Also called in SetupPlayerInputComponent because of a potential race condition.

    BindASCInput();

    AttributeSetBase = PS->GetAttributeSetBase();

    InitializeAttributes();

    AGTPlayerController *PC = Cast<AGTPlayerController>(GetController());
    if (PC)
    {
        PC->CreateHUD();
    }

    InitializeFloatingStatusBar();

    AbilitySystemComponent->SetTagMapCount(DeadTag, 0);

    SetHealth(GetMaxHealth());
    SetMana(GetMaxMana());
    SetStamia(GetMaxStamina());
}


void AGTHeroCharacter::BindASCInput()
{
    if (!ASCInputBound && AbilitySystemComponent.IsValid() && IsValid(InputComponent))
    {
        //FTopLevelAssetPath  资产路径
        FTopLevelAssetPath AbilityEnumAssetPath = FTopLevelAssetPath(FName("/Script/GASTest"), FName("EGTAbilityInputID"));
        AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent,
                                                                      FGameplayAbilityInputBinds(
                                                                          FString("ConfirmTarget"), FString("CancelTarget"),
                                                                          AbilityEnumAssetPath,
                                                                          static_cast<int32>(EGTAbilityInputID::Confirm),
                                                                          static_cast<int32>(EGTAbilityInputID::Cancel)));

        ASCInputBound = true;
    }
}

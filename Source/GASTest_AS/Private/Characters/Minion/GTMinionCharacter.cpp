// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/Minion/GTMinionCharacter.h"

#include "Characters/Abilities/GTAbilitySystemComponent.h"
#include "Characters/Abilities/AttributeSets/GTAttributeSetBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/GTFloatingStatusBarWidget.h"

// Sets default values
AGTMinionCharacter::AGTMinionCharacter(const class FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    HardRefAbilitySystemComponent = CreateDefaultSubobject<UGTAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    HardRefAbilitySystemComponent->SetIsReplicated(true);

    HardRefAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

    AbilitySystemComponent = HardRefAbilitySystemComponent;

    HardRefAttributeSetBase = CreateDefaultSubobject<UGTAttributeSetBase>(TEXT("AttributeSetBase"));

    AttributeSetBase = HardRefAttributeSetBase;

    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

    UIFloatingStatusBarComponent = CreateDefaultSubobject<UWidgetComponent>(FName("UIFloatingStatusBarComponent"));
    UIFloatingStatusBarComponent->SetupAttachment(RootComponent);
    UIFloatingStatusBarComponent->SetRelativeLocation(FVector(0, 0, 120));
    // EWidgetSpace::Screen 表示该 UI 组件将在屏幕空间中绘制，而不是世界空间。这意味着无论摄像机如何移动，UI
    // 组件都会保持在屏幕上的固定位
    UIFloatingStatusBarComponent->SetWidgetSpace(EWidgetSpace::Screen);
    // SetDrawSize: 设置 UWidgetComponent 的绘制大小。这里设置的大小是 (500, 500) 像素，即 UI
    // 组件在屏幕上显示的宽度和高度分别为 500 像素
    UIFloatingStatusBarComponent->SetDrawSize(FVector2D(500, 500));

    UIFloatingStatusBarClass = StaticLoadClass(
        UObject::StaticClass(), nullptr, TEXT("/Game/GASTest/UI/UI_FloatingStatusBar_Minion.UI_FloatingStatusBar_Minion_C"));
    if (!UIFloatingStatusBarClass)
    {
        UE_LOG(LogTemp, Error,
               TEXT("%s() Failed to find UIFloatingStatusBarClass. If it was moved, please update the reference location in "
                    "C++."),
               *FString(__FUNCTION__));
    }
}

// Called when the game starts or when spawned
void AGTMinionCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (!AbilitySystemComponent.IsValid())
    {
        return;
    }

    AbilitySystemComponent->InitAbilityActorInfo(this, this);
    InitializeAttributes();
    AddStartupEffects();
    AddCharacterAbilities();

    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC && PC->IsLocalPlayerController())
    {
        if (UIFloatingStatusBarClass)
        {
            UIFloatingStatusBar = CreateWidget<UGTFloatingStatusBarWidget>(PC, UIFloatingStatusBarClass);
            if (UIFloatingStatusBar && UIFloatingStatusBarComponent)
            {
                UIFloatingStatusBarComponent->SetWidget(UIFloatingStatusBar);

                UIFloatingStatusBar->SetHealthPercentage(GetHealth() / GetMaxHealth());
                UIFloatingStatusBar->SetCharacterName(CharacterName);
            }
        }
    }

    HealthChangedDelegateHandle =
        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute())
            .AddUObject(this, &AGTMinionCharacter::HealthChanged);

    AbilitySystemComponent
        ->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun")),
                                   EGameplayTagEventType::NewOrRemoved)
        .AddUObject(this, &AGTMinionCharacter::StunTagChanged);
}

void AGTMinionCharacter::HealthChanged(const FOnAttributeChangeData& Data)
{
    float Health = Data.NewValue;

    if (UIFloatingStatusBar)
    {
        UIFloatingStatusBar->SetHealthPercentage(Health / GetMaxHealth());
    }

    if (!IsAlive() && !AbilitySystemComponent->HasMatchingGameplayTag(DeadTag))
    {
        Die();
    }
}

void AGTMinionCharacter::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
    if (NewCount > 0)
    {
        FGameplayTagContainer AbilityTagsToCancel;
        AbilityTagsToCancel.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability")));

        FGameplayTagContainer AbilityTagsToIgnore;
        AbilityTagsToIgnore.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.NotCanceledByStun")));

        AbilitySystemComponent->CancelAbilities(&AbilityTagsToCancel,&AbilityTagsToIgnore);
    }
}

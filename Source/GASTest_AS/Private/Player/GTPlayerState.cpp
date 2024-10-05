// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/GTPlayerState.h"

#include "Characters/Abilities/AttributeSets/GTAttributeSetBase.h"
#include "Characters/Hero/GTHeroCharacter.h"
#include "Characters/Abilities/GTAbilitySystemComponent.h"
#include "Player/GTPlayerController.h"
#include "UI/GTFloatingStatusBarWidget.h"
#include "UI/GTHUDWidget.h"
#include "Windows/AllowWindowsPlatformTypes.h"

AGTPlayerState::AGTPlayerState()
{
    //创建技能系统组件，并设置其显式复制（确保在网络中被正确同步）
    AbilitySystemComponent = CreateDefaultSubobject<UGTAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    AbilitySystemComponent->SetIsReplicated(true);

    //设置复制模式为混合模式（在这种模式下，只有发送给自己的游戏效果会被复制，而发送给模拟代理的游戏效果不会被复制。这有助于减少网络流量，同时仍然保持关键信息的同步（如属性、游戏标签和游戏提示））
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

    AttributeSetBase = CreateDefaultSubobject<UGTAttributeSetBase>(TEXT("AttributeSetBase"));

    //设置PlayerState的网络更新频率。默认情况下，PlayerState的更新频率非常低，这可能会导致能力系统中的感知延迟。
    NetUpdateFrequency = 20.0f;

    //请求并获取名为 "State.Dead" 的游戏标签，并将其存储在 DeadTag 中。这样可以在后续的逻辑中方便地使用这个标签，而不需要每次都请求它
    DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
}

UAbilitySystemComponent *AGTPlayerState::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

UGTAttributeSetBase *AGTPlayerState::GetAttributeSetBase() const
{
    return AttributeSetBase;
}

bool AGTPlayerState::IsAlive() const
{
    return GetHealth() > 0.0f;
}

void AGTPlayerState::ShowAbilityConfrimCancelText(bool ShowText)
{
    AGTPlayerController *PC = Cast<AGTPlayerController>(GetOwner());
    if (PC)
    {
        UGTHUDWidget *HUD = PC->GetHUD();
        if (HUD)
        {
            HUD->ShowAbilityConfirmCancelText(ShowText);
        }
    }
}

float AGTPlayerState::GetHealth() const
{
    return AttributeSetBase->GetHealth();
}

float AGTPlayerState::GetMaxHealth() const
{
    return AttributeSetBase->GetMaxHealth();
}

float AGTPlayerState::GetHealthRegenRate() const
{
    return AttributeSetBase->GetHealthRegenRate();
}

float AGTPlayerState::GetMana() const
{
    return AttributeSetBase->GetMana();
}

float AGTPlayerState::GetMaxMana() const
{
    return AttributeSetBase->GetMaxMana();
}

float AGTPlayerState::GetManaRegenRate() const
{
    return AttributeSetBase->GetManaRegenRate();
}

float AGTPlayerState::GetStamina() const
{
    // return AttributeSetBase->GetStamina();
    return 0.0f;
}

float AGTPlayerState::GetMaxStamina() const
{
    // return AttributeSetBase->GetMaxS
    return 0.0f;
}

float AGTPlayerState::GetStaminaRegenRate() const
{
    return 0.0f;
}

float AGTPlayerState::GetArmor() const
{
    return AttributeSetBase->GetArmor();
}

float AGTPlayerState::GetMoveSpeed() const
{
    return AttributeSetBase->GetMoveSpeed();
}

int32 AGTPlayerState::GetCharacterLevel() const
{
    return AttributeSetBase->GetCharacterLevel();
}

int32 AGTPlayerState::GetXP() const
{
    return AttributeSetBase->GetXP();
}

int32 AGTPlayerState::GetXPBounty() const
{
    return AttributeSetBase->GetXPBounty();
}

int32 AGTPlayerState::GetGold() const
{
    return AttributeSetBase->GetGold();
}

int32 AGTPlayerState::GetGoldBounty() const
{
    return AttributeSetBase->GetGoldBounty();
}

void AGTPlayerState::BeginPlay()
{
    Super::BeginPlay();

    if (!AbilitySystemComponent)
    {
        return;
    }

    //属性变化回调
    HealthChangedDelegateHandle = AbilitySystemComponent->
                                  GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(
                                      this, &AGTPlayerState::HealthChanged);
    MaxHealthChangedDelegateHandle = AbilitySystemComponent->
                                     GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetMaxHealthAttribute()).
                                     AddUObject(this, &AGTPlayerState::MaxHealthChanged);
    HealthRegenRateChangedDelegateHandle = AbilitySystemComponent->
                                           GetGameplayAttributeValueChangeDelegate(
                                               AttributeSetBase->GetHealthRegenRateAttribute()).AddUObject(
                                               this, &AGTPlayerState::HealthRegenRateChanged);
    ManaChangedDelegateHandle = AbilitySystemComponent->
                                GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetManaAttribute()).AddUObject(
                                    this, &AGTPlayerState::ManaChanged);
    MaxManaChangedDelegateHandle = AbilitySystemComponent->
                                   GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetMaxManaAttribute()).
                                   AddUObject(this, &AGTPlayerState::MaxManaChanged);
    ManaRegenRateChangedDelegateHandle = AbilitySystemComponent->
                                         GetGameplayAttributeValueChangeDelegate(
                                             AttributeSetBase->GetManaRegenRateAttribute()).AddUObject(
                                             this, &AGTPlayerState::ManaRegenRateChanged);
    // StaminaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetStaminaAttribute()).AddUObject(this, &AGTPlayerState::HealthChanged);
    // HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &AGTPlayerState::HealthChanged);
    // HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &AGTPlayerState::HealthChanged);
    XPChangedDelegateHandle = AbilitySystemComponent->
                              GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetXPAttribute()).AddUObject(
                                  this, &AGTPlayerState::XPChanged);
    GoldChangedDelegateHandle = AbilitySystemComponent->
                                GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetGoldAttribute()).AddUObject(
                                    this, &AGTPlayerState::GoldChanged);
    CharacterLevelChangedDelegateHandle = AbilitySystemComponent->
                                          GetGameplayAttributeValueChangeDelegate(
                                              AttributeSetBase->GetCharacterLevelAttribute()).AddUObject(
                                              this, &AGTPlayerState::CharacterLevelChanged);
}

void AGTPlayerState::HealthChanged(const FOnAttributeChangeData &Data)
{
    float Health = Data.NewValue;

    //update floating status bar
    AGTHeroCharacter *Hero = Cast<AGTHeroCharacter>(GetPawn());
    if (Hero)
    {
        UGTFloatingStatusBarWidget *HeroFloatingStatusBar = Hero->GetFloatingStatusBar();
        if (HeroFloatingStatusBar)
        {
            HeroFloatingStatusBar->SetHealthPercentage(Health / GetMaxHealth());
        }
    }

    //Update the HUD

    // if died
    if (!IsAlive() && !AbilitySystemComponent->HasMatchingGameplayTag(DeadTag))
    {
        if (Hero)
        {
            Hero->Die();
        }
    }
}

void AGTPlayerState::MaxHealthChanged(const FOnAttributeChangeData &Data)
{
    float MaxHealth = Data.NewValue;

    //Update floating status bar
    AGTHeroCharacter *Hero = Cast<AGTHeroCharacter>(GetPawn());
    if (Hero)
    {
        UGTFloatingStatusBarWidget *HeroFloatingStatusBar = Hero->GetFloatingStatusBar();
        if (HeroFloatingStatusBar)
        {
            HeroFloatingStatusBar->SetHealthPercentage(GetHealth() / MaxHealth);
        }
    }

    //Update the HUD
    AGTPlayerController *PC = Cast<AGTPlayerController>(GetOwner());
    if (PC)
    {
        UGTHUDWidget *HUD = PC->GetHUD();
        if (HUD)
        {
            HUD->SetMaxHealth(MaxHealth);
        }
    }
}

void AGTPlayerState::HealthRegenRateChanged(const FOnAttributeChangeData &Data)
{
    float HealthRengeRate = Data.NewValue;

    AGTPlayerController *PC = Cast<AGTPlayerController>(GetOwner());
    if (PC)
    {
        UGTHUDWidget *HUD = PC->GetHUD();
        if (HUD)
        {
            HUD->SetHealthRegenRate(HealthRengeRate);
        }
    }
}

void AGTPlayerState::ManaChanged(const FOnAttributeChangeData &Data)
{
    float             Mana = Data.NewValue;
    AGTHeroCharacter *Hero = Cast<AGTHeroCharacter>(GetPawn());
    if (Hero)
    {
        UGTFloatingStatusBarWidget *HeroFloatingStatusBar = Hero->GetFloatingStatusBar();
        if (HeroFloatingStatusBar)
        {
            HeroFloatingStatusBar->SetManaPercentage(Mana / GetMaxMana());
        }
    }
}

void AGTPlayerState::MaxManaChanged(const FOnAttributeChangeData &Data)
{
    float MaxMana = Data.NewValue;

    AGTHeroCharacter *Hero = Cast<AGTHeroCharacter>(GetPawn());
    if (Hero)
    {
        UGTFloatingStatusBarWidget *HeroFloatingStatusBar = Hero->GetFloatingStatusBar();
        if (HeroFloatingStatusBar)
        {
            HeroFloatingStatusBar->SetManaPercentage(GetMana() / MaxMana);
        }
    }

    AGTPlayerController *PC = Cast<AGTPlayerController>(GetOwner());
    if (PC)
    {
        UGTHUDWidget *HUD = PC->GetHUD();
        if (HUD)
        {
            HUD->SetMaxMana(MaxMana);
        }
    }
}

void AGTPlayerState::ManaRegenRateChanged(const FOnAttributeChangeData &Data)
{
    float ManaRegenRate = Data.NewValue;

    //Update the HUD
    AGTPlayerController *PC = Cast<AGTPlayerController>(GetOwner());
    if (PC)
    {
        UGTHUDWidget *HUD = PC->GetHUD();
        if (HUD)
        {
            HUD->SetManaRegenRate(ManaRegenRate);
        }
    }
}

void AGTPlayerState::XPChanged(const FOnAttributeChangeData &Data)
{
    float XP = Data.NewValue;

    AGTPlayerController *PC = Cast<AGTPlayerController>(GetOwner());
    if (PC)
    {
        UGTHUDWidget *HUD = PC->GetHUD();
        if (HUD)
        {
            HUD->SetExperience(XP);
        }
    }
}

void AGTPlayerState::GoldChanged(const FOnAttributeChangeData &Data)
{
    float Gold = Data.NewValue;

    AGTPlayerController *PC = Cast<AGTPlayerController>(GetOwner());
    if (PC)
    {
        UGTHUDWidget *HUD = PC->GetHUD();
        if (HUD)
        {
            HUD->SetGold(Gold);
        }
    }
}

void AGTPlayerState::CharacterLevelChanged(const FOnAttributeChangeData &Data)
{
    float Level = Data.NewValue;

    AGTPlayerController *PC = Cast<AGTPlayerController>(GetOwner());
    if (PC)
    {
        UGTHUDWidget *HUD = PC->GetHUD();
        if (HUD)
        {
            HUD->SetHeroLevel(Level);
        }
    }
}

void AGTPlayerState::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
    if (NewCount > 0)
    {
        //玩家处于眩晕状态
        FGameplayTagContainer AbilityTagsToCancel;
        AbilityTagsToCancel.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability")));

        FGameplayTagContainer AbilityTagsToIgnore;
        AbilityTagsToIgnore.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.NotCanceledByStun")));

        AbilitySystemComponent->CancelAbilities(&AbilityTagsToCancel,&AbilityTagsToIgnore);
    }
}

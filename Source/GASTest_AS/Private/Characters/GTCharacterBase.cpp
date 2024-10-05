// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/GTCharacterBase.h"
#include "Characters/GTCharacterMovementComponent.h"
#include "Characters/Abilities/AttributeSets/GTAttributeSetBase.h"
#include "Characters/Abilities/GTAbilitySystemComponent.h"
#include "Characters/Abilities/GTGameplayAbitity.h"
#include "Components/CapsuleComponent.h"
#include "GASTest_AS/GASTest_AS.h"

// Sets default values
AGTCharacterBase::AGTCharacterBase(const class FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<UGTCharacterMovementComponent>(
        ACharacter::CharacterMovementComponentName))
{
    // Set this character to call Tick() every frame.  You can turn this off to
    // improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

    GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Overlap);

    bAlwaysRelevant = true;

    // Cache tags
    HitDirectionFrontTag = FGameplayTag::RequestGameplayTag(FName("Effect.HitReact.Front"));
    HitDirectionBackTag = FGameplayTag::RequestGameplayTag(FName("Effect.HitReact.Back"));
    HitDirectionRightTag = FGameplayTag::RequestGameplayTag(FName("Effect.HitReact.Right"));
    HitDirectionLeftTag = FGameplayTag::RequestGameplayTag(FName("Effect.HitReact.Left"));
    DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
    EffectRemoveOnDeathTag = FGameplayTag::RequestGameplayTag(FName("Effect.RemoveOnDeath"));
}

// Called when the game starts or when spawned
void AGTCharacterBase::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void AGTCharacterBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AGTCharacterBase::SetupPlayerInputComponent(
    UInputComponent *PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

UAbilitySystemComponent *AGTCharacterBase::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent.Get();
}

bool AGTCharacterBase::IsAlive() const
{
    return GetHealth() > 0.0f;
}

int32 AGTCharacterBase::GetAbilityLevel(EGTAbilityInputID AbilityID) const
{
    return 1;
}

void AGTCharacterBase::RemoveCharacterAbilities()
{
    /*
     *检查角色是否具有权威性
     *检查技能模块是否有效
     *检查角色是否初始化了技能
     */
    if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || !AbilitySystemComponent->
        bCharacterAbilitiesGiven)
    {
        return;
    }

    /*
     *创建数组，存储要移除的技能句柄
     *遍历可以激活的能力，判断是否在激活的能力中；且该技能是由该角色提供的（其他人给予技能的情况？）
     * 
     */
    TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;
    for (const FGameplayAbilitySpec &Spec : AbilitySystemComponent->GetActivatableAbilities())
    {
        if ((Spec.SourceObject == this) && CharacterAbilities.Contains(Spec.Ability->GetClass()))
        {
            AbilitiesToRemove.Add(Spec.Handle);
        }
    }

    //清理需要移除的技能
    for(int32 i=0;i<AbilitiesToRemove.Num();i++)
    {
        AbilitySystemComponent->ClearAbility(AbilitiesToRemove[i]);
    }

    AbilitySystemComponent->bCharacterAbilitiesGiven = false;
}

EGTHitReactDirection AGTCharacterBase::GetHitReactDirection(const FVector &ImpactPoint)
{
    const FVector &ActorLocation        = GetActorLocation();
    float          DistanceToSidePlane  = FVector::PointPlaneDist(ImpactPoint, ActorLocation, GetActorRightVector());
    float          DistanceToFrontPlane = FVector::PointPlaneDist(ImpactPoint, ActorLocation, GetActorForwardVector());

    if (FMath::Abs(DistanceToSidePlane) <= FMath::Abs(DistanceToFrontPlane))
    {
        //如果前后距离小于等于左右距离，则优先考虑前后方向

        return (DistanceToFrontPlane >= 0 ? EGTHitReactDirection::Front : EGTHitReactDirection::Back);
    }
    else
    {
        return (DistanceToSidePlane >= 0 ? EGTHitReactDirection::Right : EGTHitReactDirection::Left);
    }

    return EGTHitReactDirection::Front;
}

void AGTCharacterBase::PlayHitReact_Implementation(FGameplayTag HitDirection, AActor *DamageCauser)
{
    if (IsAlive())
    {
        if (HitDirection == HitDirectionLeftTag)
        {
            ShowHitReact.Broadcast(EGTHitReactDirection::Left);
        }
        else if (HitDirection == HitDirectionFrontTag)
        {
            ShowHitReact.Broadcast(EGTHitReactDirection::Front);
        }
        else if (HitDirection == HitDirectionRightTag)
        {
            ShowHitReact.Broadcast(EGTHitReactDirection::Right);
        }
        else if (HitDirection == HitDirectionBackTag)
        {
            ShowHitReact.Broadcast(EGTHitReactDirection::Back);
        }
    }
}

bool AGTCharacterBase::PlayHitReact_Validate(FGameplayTag HitDirection, AActor *DamageCauser)
{
    return true;
}

int32 AGTCharacterBase::GetCharacterLevel() const
{
    if (AttributeSetBase.IsValid())
    {
        return static_cast<int32>(AttributeSetBase->GetCharacterLevel());
    }

    return 0;
}

float AGTCharacterBase::GetHealth() const
{
    if (AttributeSetBase.IsValid())
    {
        return AttributeSetBase->GetHealth();
    }

    return 0.0f;
}

float AGTCharacterBase::GetMaxHealth() const
{
    if (AttributeSetBase.IsValid())
    {
        return AttributeSetBase->GetMaxHealth();
    }

    return 0.0f;
}

float AGTCharacterBase::GetMana() const
{
    if (AttributeSetBase.IsValid())
    {
        return AttributeSetBase->GetMana();
    }

    return 0.0f;
}

float AGTCharacterBase::GetMaxMana() const
{
    if (AttributeSetBase.IsValid())
    {
        return AttributeSetBase->GetMaxMana();
    }

    return 0.0f;
}

float AGTCharacterBase::GetStamia() const
{
    // if (AttributeSetBase.IsValid())
    // {
    // 	return AttributeSetBase->GetStamia();
    // }

    return 0.0f;
}

float AGTCharacterBase::GetMaxStamina() const
{
    // if (AttributeSetBase.IsValid())
    // {
    // 	return AttributeSetBase->GetMaxStamina();
    // }

    return 0.0f;
}

float AGTCharacterBase::GetMoveSpeed() const
{
    if (AttributeSetBase.IsValid())
    {
        return AttributeSetBase->GetMoveSpeed();
    }

    return 0.0f;
}

float AGTCharacterBase::GetMoveSpeedBaseValue() const
{
    if (AttributeSetBase.IsValid())
    {
        return AttributeSetBase->GetMoveSpeedAttribute().GetGameplayAttributeData(AttributeSetBase.Get())->GetBaseValue();
    }

    return 0.0f;
}

//Run on Server and all clients
void AGTCharacterBase::Die()
{
    //Only run on Server
    RemoveCharacterAbilities();

    //设置角色碰撞无效；  重力缩放设置为0； 角色速度设置为0
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetCharacterMovement()->GravityScale = 0;
    GetCharacterMovement()->Velocity     = FVector(0);

    OnCharacterDied.Broadcast(this);

    if (AbilitySystemComponent.IsValid())
    {
        //取消所有当前正在执行的技能（部分持续性技能的效果是不是不需要取消）
        AbilitySystemComponent->CancelAllAbilities();

        //从角色系统中移除所有死亡时需要被移除的效果（有些效果复活后还可以带着咯？）
        FGameplayTagContainer EffectTagsToRemove;
        EffectTagsToRemove.AddTag(EffectRemoveOnDeathTag);
        int32 NumEffectsRemoved = AbilitySystemComponent->RemoveActiveEffectsWithTags(EffectTagsToRemove);

        //向技能系统标记角色已死亡
        AbilitySystemComponent->AddLooseGameplayTag(DeadTag);
    }

    if (DeathMontage)
    {
        PlayAnimMontage(DeathMontage);
    }
    else
    {
        FinishDying();
    }
}

void AGTCharacterBase::FinishDying()
{
    Destroy();
}

//在服务器上授予技能，能力规格将被复制到拥有的客户端上
void AGTCharacterBase::AddCharacterAbilities()
{
    if(GetLocalRole()!=ROLE_Authority || !AbilitySystemComponent.IsValid() || AbilitySystemComponent->bCharacterAbilitiesGiven)
    {
        return;
    }

    for(TSubclassOf<UGTGameplayAbitity>& StartupAbility : CharacterAbilities)
    {
        AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(StartupAbility, GetAbilityLevel(StartupAbility.GetDefaultObject()->AbilityID),static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID),this));
    }

    AbilitySystemComponent->bCharacterAbilitiesGiven = true;
}

//初始化角色属性必须在服务器上运行，但是这里也运行在客户端
//服务器对客户机的复制不再重要
//属性值应当一致
void AGTCharacterBase::InitializeAttributes()
{
    if(!AbilitySystemComponent.IsValid())
    {
        return;
    }

    if(!DefaultAttributes)
    {
        UE_LOG(LogTemp, Error, TEXT("%s() Missing DefaultAttributes for %s. Please fill in the character's Blueprint."), *FString(__FUNCTION__), *GetName());
        return;
    }

    //创建一个EffectContext，并与角色关联(效果上下文）
    FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
    EffectContext.AddSourceObject(this);

    //构建效果规范，填充角色等级等必要信息
    FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributes, GetCharacterLevel(),EffectContext);
    if(NewHandle.IsValid())
    {
        //将效果规范应用到AbilitySystemComponent上
        FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent.Get());

        // if(ActiveGEHandle.WasSuccessfullyApplied())
        // {
        //     
        // }
    }

    /*
     *相关class整理
     *FGameplayEffectContextHandle 封装GE执行上下文的信息，当GE被触发时，会创建一个FGameplayEffectContextHandle实例来记录与该效果执行相关的信息，比如执行者，目标，执行位置，之后传递给后续处理逻辑
     *FGameplayEffectSpecHandle  GE规格的句柄，封装了GE在应用前所有规格信息，包含GE所有的参数和配置。（在GE应用于目标之前，创建一个包含所有必要信息的规格，在实际应用GE前对规格进行修改或验证）
     * FActiveGameplayEffectHandle 代表已经应用到目标上的GE的句柄，主要用来管理活跃的GE，允许开发者对生命周期进行控制，比如提前结束效果，刷新（或延迟）持续时间等。
     */
}

void AGTCharacterBase::AddStartupEffects()
{
    if(GetLocalRole()!=ROLE_Authority ||! AbilitySystemComponent.IsValid() || AbilitySystemComponent->bStartupEffectsApplied)
    {
        return;
    }

    FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
    EffectContext.AddSourceObject(this);

    //MakeOutgoingSpec 创建一个即将应用的游戏效果，需要指定游戏效果类（GameplayEffect），等级（这里传的是角色等级），效果上下文
    for(TSubclassOf<UGameplayEffect> GameplayEffect : StartupEffects)
    {
        FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect,GetCharacterLevel(),EffectContext);
        if(NewHandle.IsValid())
        {
            FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent.Get());
        }
    }

    AbilitySystemComponent->bStartupEffectsApplied = true;
}

//属性设置，只在特殊情况下使用，比如重生等。否则应该使用GE去改变属性
//这些改变属性的基本值
    
void AGTCharacterBase::SetHealth(float Health)
{
    if(AttributeSetBase.IsValid())
    {
        AttributeSetBase->SetHealth(Health);
    }
    
}

void AGTCharacterBase::SetMana(float Mana)
{
    if(AttributeSetBase.IsValid())
    {
        AttributeSetBase->SetMana(Mana);
    }
    
}

void AGTCharacterBase::SetStamia(float Stamina)
{
    // if(AttributeSetBase.IsValid())
    // {
    //     AttributeSetBase->SetStamia(Mana);
    // }
    
}
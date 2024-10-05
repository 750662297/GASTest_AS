// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/Hero/Abilities/GTGA_FireGun.h"

#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Characters/Abilities/AbilityTasks/GTAT_PlayMontageAndWaitForEvent.h"
#include "Characters/Hero/GTHeroCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"

UGTGA_FireGun::UGTGA_FireGun()
{
    /*设置实例化策略
     *NonInstanced
     *这种策略表示该能力永远不会被实例化。任何执行该能力的操作都是基于类的默认对象(适用于不需要保存状态的能力，或者状态可以完全通过外部数据来管理的能力)
     *(biru
     *InstancedPerActor  标识每个使用该能力的Actor都会一个独立的UGTGA_FireGun实例（适用于每个Actor有自己独立状态的能力）
     *InstancedPerExecution
     *每次执行该能力时都会创建一个新的实例。虽然复制是可能的，但不推荐这样做（适用于一次性使用的、不需要长期保存状态的能力，例如瞬发技能。）
     *
     */
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    FGameplayTag Ability1Tag = FGameplayTag::RequestGameplayTag(FName("Ability.Skill.Ability1"));
    // AbilityTags 用于标识该能力具有的标签
    AbilityTags.AddTag(Ability1Tag);
    // ActivationOwnedTags 表示在激活该能力时，Actor 会获得这些标签。这些标签可以用于触发其他依赖于这些标签的逻辑
    ActivationOwnedTags.AddTag(Ability1Tag);

    // ActivationBlockedTags 表示在激活该能力时，如果 Actor
    // 已经拥有这些标签，则该能力不能被激活。这可以用来防止某些能力在同一时间被多次激活
    ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill")));

    Range  = 1000.0f;
    Damage = 12.0f;
}

void UGTGA_FireGun::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo,
                                    const FGameplayEventData            *TriggerEventData)
{
    // 检查并提交当前的能力，CommitAbility尝试提交当前的能力实例，如果提交失败，则立即通过EndAbility结束该能力
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
    }

    UAnimMontage *MontageToPlay = FireHipMontage;  // 默认为腰射

    // 检查角色是否处于瞄准状态（State.AimDownSights 标签存在且 State.AimDownSights.Removal 标签不存在）
    if (GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(
            FGameplayTag::RequestGameplayTag(FName("State.AimDownSights")))
        && !GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(
            FGameplayTag::RequestGameplayTag(FName("State.AimDownSights.Removal"))))
    {
        MontageToPlay = FireIronsightsMontage;
    }
    
    // UGTAT_PlayMontageAndWaitForEvent *Task = UGTAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(
    //     this, NAME_None, MontageToPlay, FGameplayTagContainer(), 1.0f, NAME_None, false, 1.0f);
    //
    // Task->OnBlendOut.AddDynamic(this, &UGTGA_FireGun::OnCompleted);
    // Task->OnCompleted.AddDynamic(this, &UGTGA_FireGun::OnCompleted);
    // Task->OnInterrupted.AddDynamic(this, &UGTGA_FireGun::OnCancelled);
    // Task->OnCancelled.AddDynamic(this, &UGTGA_FireGun::OnCancelled);
    // Task->EventReceived.AddDynamic(this, &UGTGA_FireGun::EventReceived);

    UGTAT_PlayMontageAndWaitForEvent *Task =
    UGTAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(
        this, NAME_None, MontageToPlay, FGameplayTagContainer(), 1.0f,
        NAME_None, false, 1.0f);
    Task->OnBlendOut.AddDynamic(this, &UGTGA_FireGun::OnCompleted);
    Task->OnCompleted.AddDynamic(this, &UGTGA_FireGun::OnCompleted);
    Task->OnInterrupted.AddDynamic(this, &UGTGA_FireGun::OnCancelled);
    Task->OnCancelled.AddDynamic(this, &UGTGA_FireGun::OnCancelled);
    Task->EventReceived.AddDynamic(this, &UGTGA_FireGun::EventReceived);

    // ReadyForActivation()是在c++中激活AbilityTask的方法。Blueprint有来自K2Node_LatentGameplayTaskCall的魔法，它将自动调用ReadyForActivation()。
    Task->ReadyForActivation();
}

void UGTGA_FireGun::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGTGA_FireGun::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGTGA_FireGun::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
    if (EventTag == FGameplayTag::RequestGameplayTag(FName("Event.Montage.EndAbility")))
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
        return;
    }

    // Only spawn projectiles on the Server.
    // Predicting projectiles is an advanced topic not covered in this example.
    if (GetOwningActorFromActorInfo()->GetLocalRole() == ROLE_Authority
        && EventTag == FGameplayTag::RequestGameplayTag(FName("Event.Montage.SpawnProjectile")))
    {
        AGTHeroCharacter *Hero = Cast<AGTHeroCharacter>(GetAvatarActorFromActorInfo());
        if (!Hero)
        {
            EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
            return;
        }

        // 获取枪口位置作为发射起点Start，并基于相机视角前方一定距离的位置作为终点End，使用FindLookAtRotation计算从起点到终点的方向旋转
        FVector   Start = Hero->GetGunComponent()->GetSocketLocation(FName("Muzzle"));
        FVector   End   = Hero->GetCameraBoom()->GetComponentLocation() + Hero->GetFollowCamera()->GetForwardVector() * Range;
        FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(Start, End);

        // 创建DamageEffectSpecHandle，并设置伤害量
        FGameplayEffectSpecHandle DamageEffectSpecHandle =
            MakeOutgoingGameplayEffectSpec(DamageGameplayEffect, GetAbilityLevel());

        DamageEffectSpecHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")),
                                                                   Damage);

        // 设置枪口变换MuzzleTransform，包括位置，旋转和平移缩放
        FTransform MuzzleTransform = Hero->GetGunComponent()->GetSocketTransform(FName("Muzzle"));
        MuzzleTransform.SetRotation(Rotation.Quaternion());
        MuzzleTransform.SetScale3D(FVector(1.0f));

        //指定碰撞处理方式
        FActorSpawnParameters SpawnParameters;
        SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        //使用SpawnActorDeferred延迟生成投射物Projectile对象
        //这样可以在最终放置前配置额外属性如伤害效果和飞行距离
        AGTProjectileBase* Projectile = GetWorld()->SpawnActorDeferred<AGTProjectileBase>(ProjectileClass, MuzzleTransform, GetOwningActorFromActorInfo(),Hero, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
        Projectile->DamageEffectSpecHandle = DamageEffectSpecHandle;
        Projectile->Range = Range;
        
        Projectile->FinishSpawning(MuzzleTransform);
    }
}

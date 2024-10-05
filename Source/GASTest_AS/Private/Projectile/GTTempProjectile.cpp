// Fill out your copyright notice in the Description page of Project Settings.

#include "Projectile/GTTempProjectile.h"
#include "AbilitySystemComponent.h"
#include "Characters/GTCharacterBase.h"
#include "Kismet/KismetSystemLibrary.h"

#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AGTTempProjectile::AGTTempProjectile()
    :Super()
{
    Collision = CreateDefaultSubobject<USphereComponent>(FName("Collision"));
    RootComponent = Collision;
    // Collision->SetupAttachment(RootComponent);
    
    Sphere = CreateDefaultSubobject<UStaticMeshComponent>(FName("Sphere"));
    Sphere->SetupAttachment(Collision);
}

// Called when the game starts or when spawned
void AGTTempProjectile::BeginPlay()
{
    Super::BeginPlay();

    float InitSpeed = ProjectileMovementComponent->InitialSpeed;
    float Temp      = Range / InitSpeed;
    SetLifeSpan(Temp);
    
    Collision->OnComponentBeginOverlap.AddDynamic(this, &AGTTempProjectile::OnComBeginOverlap);
    // Sphere->OnComponentBeginOverlap.AddDynamic(this, &AGTTempProjectile::OnComBeginOverlap);
}

void AGTTempProjectile::OnComBeginOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
                                          UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                          const FHitResult &SweepResult)
{
    if (OtherActor == GetInstigator())
    {
        return;
    }

    if (!OtherActor->HasAuthority())
    {
        Destroy();
        return;
    }

    AGTCharacterBase *Character = Cast<AGTCharacterBase>(OtherActor);
    if (!IsValid(Character))
    {
        Destroy();
        return;
    }

    UAbilitySystemComponent *ASC = Character->GetAbilitySystemComponent();
    if (!IsValid(ASC))
    {
        return;
    }

    FVector StartLocation = GetActorLocation();
    // StartLocation += FVector(0, 0, 1);
    FVector          EndLocation = Character->GetActorLocation();
    TArray<AActor *> Temp;

    FHitResult Result;
    bool       result = UKismetSystemLibrary::LineTraceSingle(GetWorld(), StartLocation, EndLocation,
                                                              UEngineTypes::ConvertToTraceType(ECC_Pawn), false, Temp,
                                                              EDrawDebugTrace::ForDuration, Result, true);
    if (!result)
    {
        Destroy();
        return;
    }

    
    FinalHit = Result;
    // FinalHit.bStartPenetrating = false;
    
    if (FinalHit.IsValidBlockingHit())
    {
        // 获取 FGameplayEffectSpec
        FGameplayEffectSpec* Spec = DamageEffectSpecHandle.Data.Get();

        // 从 Spec 中获取现有的 FGameplayEffectContext
        FGameplayEffectContextHandle ContextHandle = Spec->GetContext();
        FGameplayEffectContext* Context = ContextHandle.Get();

        // 将 HitResult 添加到现有上下文中
        Context->AddHitResult(FinalHit);
        
        ASC->BP_ApplyGameplayEffectSpecToSelf(DamageEffectSpecHandle);
    }


    Destroy();
}

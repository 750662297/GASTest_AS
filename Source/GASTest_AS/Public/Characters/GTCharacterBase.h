// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GASTest_AS/GASTest_AS.h"
#include "GTCharacterBase.generated.h"

//在受到攻击后，广播受击方向
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterBaseHitReactDelegate, EGTHitReactDirection,Direction);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterDiedDelegate, AGTCharacterBase*, Character);


UCLASS()
class GASTEST_AS_API AGTCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGTCharacterBase(const class FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(
		class UInputComponent* PlayerInputComponent) override;

    //BlueprintAssignable允许在C++中定义委托，并在蓝图中绑定回调函数 （允许在动画蓝图中设置角色受到攻击时的反应方向，通过这个委托，可以控制角色在不同情况下显示不同的受伤动画
    UPROPERTY(BlueprintAssignable, Category = "GASTest|GTCharacter")
    FCharacterBaseHitReactDelegate ShowHitReact;

    UPROPERTY(BlueprintAssignable, Category = "GASTest|GTCharacter")
    FCharacterDiedDelegate OnCharacterDied;
    
    UFUNCTION(BlueprintCallable, Category = "GASTest|GTCharacter")
    virtual bool IsAlive() const;
    
    UFUNCTION(BlueprintCallable, Category = "GASTest|GTCharacter")
    virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

    UFUNCTION(BlueprintCallable, Category = "GASTest|GTCharacter")
    virtual  int32 GetAbilityLevel(EGTAbilityInputID AbilityID) const;

    //移除所有角色技能，只能由服务器调用，移除服务器上的技能会同步地移除客户端上的技能
    virtual void RemoveCharacterAbilities();
    
    UFUNCTION(BlueprintCallable)
    EGTHitReactDirection GetHitReactDirection(const FVector& ImpactPoint);
    
	/*
	 *NetMulticast  元数据标签，它指示该函数将在所有已连接的客户端上执行，包括服务器端。这通常用于那些不需要返回值并且不会影响游戏状态的操作，例如播放动画或声音效果
	 *Reliable 即使在网络条件不佳的情况下，该函数也会确保被调用。如果某个网络包丢失了，系统会尝试重新发送直到成功。这对于需要确保动作一致性的场景非常重要，比如播放打击动画
	 *WithValidation 该函数需要验证。这意味着除了定义了一个实际执行操作的实现函数外，还需要定义一个验证函数，用于检查传入的参数是否有效。如果验证失败，则不会调用实现函数
	 */
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	virtual void PlayHitReact(FGameplayTag HitDirection, AActor* DamageCauser);
	virtual void PlayHitReact_Implementation(FGameplayTag HitDirection, AActor* DamageCauser);
	virtual bool PlayHitReact_Validate(FGameplayTag HitDirection, AActor* DamageCauser);


    // Getters for attributes from GTAttributeSetBase

    UFUNCTION(BlueprintCallable, Category = "GASTest|GTCharacter|Attributes")
    int32 GetCharacterLevel() const;

	UFUNCTION(BlueprintCallable, Category = "GASTest|GTCharacter|Attributes")
	float GetHealth() const;

    UFUNCTION(BlueprintCallable, Category = "GASTest|GTCharacter|Attributes")
    float GetMaxHealth() const;

    UFUNCTION(BlueprintCallable, Category = "GASTest|GTCharacter|Attributes")
    float GetMana() const;

    UFUNCTION(BlueprintCallable, Category = "GASTest|GTCharacter|Attributes")
    float GetMaxMana() const;

    UFUNCTION(BlueprintCallable, Category = "GASTest|GTCharacter|Attributes")
    float GetStamia() const;
    
    UFUNCTION(BlueprintCallable, Category = "GASTest|GTCharacter|Attributes")
    float GetMaxStamina() const;

    UFUNCTION(BlueprintCallable, Category = "GASTest|GTCharacter|Attributes")
    float GetMoveSpeed() const;

    UFUNCTION(BlueprintCallable, Category = "GASTest|GTCharacter|Attributes")
    float GetMoveSpeedBaseValue() const;

    virtual void Die();

    UFUNCTION(BlueprintCallable, Category = "GASTest|GTCharacter")
    virtual void FinishDying();
    
protected:
	TWeakObjectPtr<class UGTAttributeSetBase> AttributeSetBase;
	TWeakObjectPtr<class UGTAbilitySystemComponent> AbilitySystemComponent;

    FGameplayTag HitDirectionFrontTag;
    FGameplayTag HitDirectionBackTag;
    FGameplayTag HitDirectionRightTag;
    FGameplayTag HitDirectionLeftTag;
    FGameplayTag DeadTag;
    FGameplayTag EffectRemoveOnDeathTag;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASTest|GTCharacter")
    FText CharacterName;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GASTest|GTCharacter")
    UAnimMontage* DeathMontage;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASTest|GTCharacter")
    TArray<TSubclassOf<class UGTGameplayAbility>> CharacterAbilities;

    //初始化角色的默认属性
    //即时的GE，覆盖在刷出/重生时重置的属性值
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASTest|GTCharacter")
    TSubclassOf<class UGameplayEffect> DefaultAttributes;

    //只在启动时应用一次
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASTest|GTCharacter")
    TArray<TSubclassOf<class UGameplayEffect>> StartupEffects;


    //在服务器上授予技能，能力规格将被复制到拥有的客户端上
    virtual void AddCharacterAbilities();

    //初始化角色属性必须在服务器上运行，但是这里也运行在客户端
    //服务器对客户机的复制不再重要
    //属性值应当一致
    virtual void InitializeAttributes();

    virtual void AddStartupEffects();


    //属性设置，只在特殊情况下使用，比如重生等。否则应该使用GE去改变属性
    //这些改变属性的基本值
    
    virtual void SetHealth(float Health);
    virtual void SetMana(float Mana);
    virtual void SetStamia(float Stamina);
};

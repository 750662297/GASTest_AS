// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Abilities/AttributeSets/GTAttributeSetBase.h"
#include "GASTest_AS/GASTest_AS.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "Characters/GTCharacterBase.h"
#include "Player/GTPlayerController.h"


UGTAttributeSetBase::UGTAttributeSetBase()
{
	//Cache tags
	HitDirectionFrontTag = FGameplayTag::RequestGameplayTag(FName("Effect.HitReact.Front"));
	HitDirectionBackTag = FGameplayTag::RequestGameplayTag(FName("Effect.HitReact.Back"));
	HitDirectionLeftTag = FGameplayTag::RequestGameplayTag(FName("Effect.HitReact.Left"));
	HitDirectionRightTag = FGameplayTag::RequestGameplayTag(FName("Effect.HitReact.Right"));
}

void UGTAttributeSetBase::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetMaxHealthAttribute())
	{
		AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
	}
	if (Attribute == GetMoveSpeedAttribute())
	{
		NewValue = FMath::Clamp<float>(NewValue, 150, 1000); //将NewValue的值限制在150-1000
	}
}

void UGTAttributeSetBase::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	//获取上下文信息
	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();

	//获取标签信息
	const FGameplayTagContainer& SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
	FGameplayTagContainer SpecAssetTags;
	Data.EffectSpec.GetAllAssetTags(SpecAssetTags);

	//获取目标角色信息
	AActor* TargetActor = nullptr;
	AController* TargetController = nullptr;
	AGTCharacterBase* TargetCharacter = nullptr;
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		TargetCharacter = Cast<AGTCharacterBase>(TargetActor);
	}

	//获取源角色信息
	AActor* SourceActor = nullptr;
	AController* SourceController = nullptr;
	AGTCharacterBase* SourceCharacter = nullptr;
	if (Source && Source->AbilityActorInfo.IsValid() && Source->AbilityActorInfo->AvatarActor.IsValid())
	{
		SourceActor = Source->AbilityActorInfo->AvatarActor.Get();
		SourceController = Source->AbilityActorInfo->PlayerController.Get();

		if (SourceController == nullptr && SourceActor != nullptr)
		{
			if (APawn* Pawn = Cast<APawn>(SourceActor))
			{
				SourceController = Pawn->GetController();
			}
		}

		if (SourceController)
		{
			SourceCharacter = Cast<AGTCharacterBase>(SourceController->GetPawn());
		}
		else
		{
			SourceCharacter = Cast<AGTCharacterBase>(SourceActor);
		}

		if (Context.GetEffectCauser())
		{
			SourceActor = Context.GetEffectCauser(); //当效果的触发者不是直接角色时不需要这一步
		}
	}

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
			if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// Try to extract a hit result
		FHitResult HitResult;
		if (Context.GetHitResult())
		{
			HitResult = *Context.GetHitResult();
		}

		// Store a local copy of the amount of damage done and clear the damage attribute
		const float LocalDamageDone = GetDamage();
		SetDamage(0.f);

		if (LocalDamageDone > 0.0f)
		{
			// If character was alive before damage is added, handle damage
			// This prevents damage being added to dead things and replaying death animations
			bool WasAlive = true;

			if (TargetCharacter)
			{
				WasAlive = TargetCharacter->IsAlive();
			}

			if (!TargetCharacter->IsAlive())
			{
				//UE_LOG(LogTemp, Warning, TEXT("%s() %s is NOT alive when receiving damage"), TEXT(__FUNCTION__), *TargetCharacter->GetName());
			}

			// Apply the health change and then clamp it
			const float NewHealth = GetHealth() - LocalDamageDone;
			SetHealth(FMath::Clamp(NewHealth, 0.0f, GetMaxHealth()));

			if (TargetCharacter && WasAlive)
			{
				// This is the log statement for damage received. Turned off for live games.
				//UE_LOG(LogTemp, Log, TEXT("%s() %s Damage Received: %f"), TEXT(__FUNCTION__), *GetOwningActor()->GetName(), LocalDamageDone);

				// Play HitReact animation and sound with a multicast RPC.
				const FHitResult* Hit = Data.EffectSpec.GetContext().GetHitResult();

				if (Hit)
				{
					EGTHitReactDirection HitDirection = TargetCharacter->GetHitReactDirection(
						Data.EffectSpec.GetContext().GetHitResult()->Location);
					switch (HitDirection)
					{
					case EGTHitReactDirection::Left:
						TargetCharacter->PlayHitReact(HitDirectionLeftTag, SourceCharacter);
						break;
					case EGTHitReactDirection::Front:
						TargetCharacter->PlayHitReact(HitDirectionFrontTag, SourceCharacter);
						break;
					case EGTHitReactDirection::Right:
						TargetCharacter->PlayHitReact(HitDirectionRightTag, SourceCharacter);
						break;
					case EGTHitReactDirection::Back:
						TargetCharacter->PlayHitReact(HitDirectionBackTag, SourceCharacter);
						break;
					}
				}
				else
				{
					// No hit result. Default to front.
					TargetCharacter->PlayHitReact(HitDirectionFrontTag, SourceCharacter);
				}

				// Show damage number for the Source player unless it was self damage
				if (SourceActor != TargetActor)
				{
					AGTPlayerController* PC = Cast<AGTPlayerController>(SourceController);
					if (PC)
					{
						PC->ShowDamageNumber(LocalDamageDone, TargetCharacter);
					}
				}

				if (!TargetCharacter->IsAlive())
				{
					// TargetCharacter was alive before this damage and now is not alive, give XP and Gold bounties to Source.
					// Don't give bounty to self.
					if (SourceController != TargetController)
					{
						// Create a dynamic instant Gameplay Effect to give the bounties
						UGameplayEffect* GEBounty = NewObject<UGameplayEffect>(
							GetTransientPackage(), FName(TEXT("Bounty")));
						GEBounty->DurationPolicy = EGameplayEffectDurationType::Instant;

						int32 Idx = GEBounty->Modifiers.Num();
						GEBounty->Modifiers.SetNum(Idx + 2);

						FGameplayModifierInfo& InfoXP = GEBounty->Modifiers[Idx];
						InfoXP.ModifierMagnitude = FScalableFloat(GetXPBounty());
						InfoXP.ModifierOp = EGameplayModOp::Additive;
						InfoXP.Attribute = UGTAttributeSetBase::GetXPAttribute();

						FGameplayModifierInfo& InfoGold = GEBounty->Modifiers[Idx + 1];
						InfoGold.ModifierMagnitude = FScalableFloat(GetGoldBounty());
						InfoGold.ModifierOp = EGameplayModOp::Additive;
						InfoGold.Attribute = UGTAttributeSetBase::GetGoldAttribute();

						Source->ApplyGameplayEffectToSelf(GEBounty, 1.0f, Source->MakeEffectContext());
					}
				}
			}
		}
	} // Damage

	}
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.0f, GetMaxMana()));
	}
}

void UGTAttributeSetBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
}

void UGTAttributeSetBase::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute,
                                                      const FGameplayAttributeData& MaxAttribute, float NewMaxValue,
                                                      const FGameplayAttribute& AffectedAttributeProperty)
{
	UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();

	if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && AbilityComp)
	{
		const float CurrentValue = AffectedAttribute.GetCurrentValue();
		float NewDelta = (CurrentMaxValue > 0.f)
			                 ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentMaxValue
			                 : NewMaxValue;

		AbilityComp->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
	}
}

void UGTAttributeSetBase::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGTAttributeSetBase, Health, OldHealth);
}

void UGTAttributeSetBase::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGTAttributeSetBase, MaxHealth, OldMaxHealth);
}

void UGTAttributeSetBase::OnRep_HealthRegenRate(const FGameplayAttributeData& OldHealthRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGTAttributeSetBase, HealthRegenRate, OldHealthRegenRate);
}

void UGTAttributeSetBase::OnRep_Armor(const FGameplayAttributeData& OldArmor)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGTAttributeSetBase, Armor, OldArmor);
}

void UGTAttributeSetBase::OnRep_Mana(const FGameplayAttributeData& OldMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGTAttributeSetBase, Mana, OldMana);
}

void UGTAttributeSetBase::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGTAttributeSetBase, MaxMana, OldMaxMana);
}

void UGTAttributeSetBase::OnRep_ManaRegenRate(const FGameplayAttributeData& OldManaRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGTAttributeSetBase, ManaRegenRate, OldManaRegenRate);
}

void UGTAttributeSetBase::OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGTAttributeSetBase, MoveSpeed, OldMoveSpeed);
}

void UGTAttributeSetBase::OnRep_CharacterLevel(const FGameplayAttributeData& OldCharacterLevel)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGTAttributeSetBase, CharacterLevel, OldCharacterLevel);
}

void UGTAttributeSetBase::OnRep_XP(const FGameplayAttributeData& OldXP)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGTAttributeSetBase, XP, OldXP);
}

void UGTAttributeSetBase::OnRep_XPBounty(const FGameplayAttributeData& OldXPBounty)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGTAttributeSetBase, XPBounty, OldXPBounty);
}

void UGTAttributeSetBase::OnRep_Gold(const FGameplayAttributeData& OldGold)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGTAttributeSetBase, Gold, OldGold);
}

void UGTAttributeSetBase::OnRep_GoldBounty(const FGameplayAttributeData& OldGoldBounty)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGTAttributeSetBase, GoldBounty, OldGoldBounty);
}

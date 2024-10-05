// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GTHUDWidget.h"

void UGTHUDWidget::AddCooldownInfo(const FText& AbilityName, FGameplayTag AbilityTag, float CooldownDuration,
                                   float currentTime)
{
	if (UWorld* World = GetWorld())
	{
		FTimerManager& TimerManager = World->GetTimerManager();
		if (!TimerManager.IsTimerActive(HUDTimerHandle))
		{
			TimerManager.SetTimer(HUDTimerHandle, this, &UGTHUDWidget::OnTimerout, TimerInvertal, true);
		}
	}

	if (CooldownAbilities.Find(AbilityTag))
	{
		UpdateCooldownInfo(AbilityName, AbilityTag, CooldownDuration, currentTime);
	}
	else
	{
		AddCooldownItem(AbilityName, AbilityTag, CooldownDuration, currentTime);
	}
}

void UGTHUDWidget::RemoveCooldown(FGameplayTag AbilityTag)
{
	CooldownAbilities.Remove(AbilityTag);

	TArray<FGameplayTag> Array;
	Array.Add(AbilityTag);
	RemoveCooldownItems(Array);
}

void UGTHUDWidget::UpdateBufferNumber(FGameplayTag AbilityTag, int32 Number)
{
	if(!CooldownAbilities.Find(AbilityTag))
	{
		return;
	}

	CooldownAbilities[AbilityTag]->BuffNumber = Number;
	const TArray<UUserWidget*>& Array = ListView->GetDisplayedEntryWidgets();
	for (UUserWidget* Item : Array)
	{
		UGTCooldownItemWidget* ItemWidget = Cast<UGTCooldownItemWidget>(Item);
		if (!ItemWidget || ItemWidget->ItemTag!=AbilityTag)
		{
			continue;
		}

		ItemWidget->UpdateBufferNumber(Number);
	}
}

void UGTHUDWidget::UpdateCooldownInfo(const FText& AbilityName, FGameplayTag AbilityTag, float CooldownDuration,
                                      float CurrentTime)
{
	if (CurrentTime <= 0)
	{
		RemoveCooldown(AbilityTag);

		return;
	}

	CooldownAbilities[AbilityTag]->AbilityName = AbilityName;
	CooldownAbilities[AbilityTag]->Duration = CooldownDuration;
	CooldownAbilities[AbilityTag]->Current = CurrentTime;
	CooldownAbilities[AbilityTag]->AbilityTag = AbilityTag;

	TArray<FGameplayTag> Array;
	Array.Add(AbilityTag);

	UpdateCooldownItems(Array);
}

void UGTHUDWidget::UpdateCooldownItems(TArray<FGameplayTag> ItemsToUpdate)
{
	const TArray<UUserWidget*>& Array = ListView->GetDisplayedEntryWidgets();
	for (UUserWidget* Item : Array)
	{
		UGTCooldownItemWidget* ItemWidget = Cast<UGTCooldownItemWidget>(Item);
		if (!ItemWidget)
		{
			continue;
		}

		if (ItemsToUpdate.IsEmpty())
		{
			break;
		}
		if (!ItemsToUpdate.Contains(ItemWidget->ItemTag))
		{
			continue;
		}

		ItemWidget->UpdateItemInfo(CooldownAbilities[ItemWidget->ItemTag]);
		ItemsToUpdate.Remove(ItemWidget->ItemTag);
	}
}

void UGTHUDWidget::RemoveCooldownItems(TArray<FGameplayTag> ItemsToRemove)
{
	int Number = ListView->GetNumItems();
	for (int i = Number - 1; i >= 0; i--)
	{
		if(ItemsToRemove.IsEmpty())
		{
			break;
		}
		
		UGTCooldownItemData* Data = Cast<UGTCooldownItemData>(ListView->GetItemAt(i));
		if (!Data)
		{
			continue;
		}

		if(ItemsToRemove.Contains(Data->AbilityTag))
		{
			ListView->RemoveItem(Data);
			ItemsToRemove.Remove(Data->AbilityTag);
		}
	}
}

void UGTHUDWidget::AddCooldownItem(const FText& Name, FGameplayTag AbilityTag, float Duration, float Current)
{
	UGTCooldownItemData* Info = NewObject<UGTCooldownItemData>(this);
	Info->AbilityName = Name;
	Info->AbilityTag = AbilityTag;
	Info->Duration = Duration;
	Info->Current = Current;

	CooldownAbilities.Add(AbilityTag, Info);

	ListView->AddItem(Info);
}

void UGTHUDWidget::OnTimerout()
{
	if (CooldownAbilities.IsEmpty())
	{
		return;
	}

	TArray<FGameplayTag> ItemsToUpdate;
	TArray<FGameplayTag> ItemsToRemove;
	for (auto& pair : CooldownAbilities)
	{
		if (pair.Value->Current + TimerInvertal >= pair.Value->Duration)
		{
			ItemsToRemove.Add(pair.Key);
		}
		else
		{
			pair.Value->Current += TimerInvertal;
			ItemsToUpdate.Add(pair.Key);
		}
	}

	for (const FGameplayTag& Tag : ItemsToRemove)
	{
		CooldownAbilities.Remove(Tag);
	}
	
	UpdateCooldownItems(ItemsToUpdate);
	RemoveCooldownItems(ItemsToRemove);
}

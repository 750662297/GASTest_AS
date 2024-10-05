// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GTCooldownItemWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UGTCooldownItemWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UGTCooldownItemWidget::UpdateItemInfo(const UGTCooldownItemData* Info)
{
	ItemTag = Info->AbilityTag;
	TextName->SetText(Info->AbilityName);

	UpdateBufferNumber(Info->BuffNumber);
	
	float Temp = Info->Duration-Info->Current;
	TextTime->SetText(FText::AsNumber(Temp));

	ProgressBar->SetPercent(Temp/Info->Duration);
}

void UGTCooldownItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	UGTCooldownItemData* ListItemData = Cast<UGTCooldownItemData>(ListItemObject);
	if (ListItemData)
	{
		// 调用自定义的 UpdateItemData 函数来更新 ListItem 显示
		UpdateItemInfo(ListItemData);
	}
}

void UGTCooldownItemWidget::UpdateBufferNumber(int32 Number)
{
	if(Number>0)
	{
		TextBufferNumber->SetText(FText::AsNumber(Number));
		TextBufferNumber->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		TextBufferNumber->SetVisibility(ESlateVisibility::Hidden);
	}
}

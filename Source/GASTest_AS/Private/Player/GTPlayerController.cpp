// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/GTPlayerController.h"

#include "AbilitySystemComponent.h"
#include "Player/GTPlayerState.h"
#include "UI/GTHUDWidget.h"
#include "UI/GTDamageTextWidgetComponent.h"

void AGTPlayerController::CreateHUD()
{
	if(UIHUDWidget)
	{
	    return;
	}

    if(!UIHUDWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("%s() Missing UIHUDWidgetClass. Please fill in on the Blueprint of the PlayerController."), *FString(__FUNCTION__));
        return;
    }

    // only create a HUD for local player
    if(!IsLocalPlayerController())
    {
        return;
    }

    // need a valid PlayerState to get attributes from
    AGTPlayerState* PS = GetPlayerState<AGTPlayerState>();
    if(!PS)
    {
        return;
    }

    UIHUDWidget  =CreateWidget<UGTHUDWidget>(this, UIHUDWidgetClass);
    UIHUDWidget->AddToViewport();

    //Set attributes
    UIHUDWidget->SetCurrentHealth(PS->GetHealth());
}

UGTHUDWidget *AGTPlayerController::GetHUD()
{
    return  UIHUDWidget;
}

void AGTPlayerController::ShowDamageNumber_Implementation(float DamageAmount, AGTCharacterBase* TargetCharacter)
{
	if(TargetCharacter && DamageNumberClass)
	{
	    UGTDamageTextWidgetComponent* DamageText = NewObject<UGTDamageTextWidgetComponent>(TargetCharacter, DamageNumberClass);
	    DamageText->RegisterComponent(); //将组件注册到UE的组件管理系统中

	    USceneComponent* Temp = TargetCharacter->GetRootComponent();
	    if(IsValid(Temp))
	    {
	        DamageText->AttachToComponent(TargetCharacter->GetRootComponent(),FAttachmentTransformRules::KeepRelativeTransform);
	        DamageText->SetDamageText(DamageAmount);
	    }

	}
}

bool AGTPlayerController::ShowDamageNumber_Validate(float DamageAmount, AGTCharacterBase* TargetCharacter)
{
	return true;
}

void AGTPlayerController::SetRespawnCountdown_Implementation(float RespawnTimeRemaining)
{
    if(UIHUDWidget)
    {
        UIHUDWidget->SetRespawnCountdown(RespawnTimeRemaining);
    }
}

bool AGTPlayerController::SetRespawnCountdown_Validate(float RespawnTimeRemaining)
{
    return true;
}


//Server only

void AGTPlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    //在控制器控制玩家后，初始化技能系统
    AGTPlayerState* PS=GetPlayerState<AGTPlayerState>();
    if(PS)
    {
        PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS,InPawn);
    }
}

void AGTPlayerController::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();

    CreateHUD();
}
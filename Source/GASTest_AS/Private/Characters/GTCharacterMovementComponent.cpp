// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/GTCharacterMovementComponent.h"

#include "AbilitySystemComponent.h"
#include "Characters/GTCharacterBase.h"

// Sets default values for this component's properties
UGTCharacterMovementComponent::UGTCharacterMovementComponent()
{
    SprintSpeedMultiplier = 1.4f;
    ADSSpeedMultiplier    = 0.5f;
}

float UGTCharacterMovementComponent::GetMaxSpeed() const
{
    AGTCharacterBase *Owner = Cast<AGTCharacterBase>(GetOwner());
    if (!Owner)
    {
        UE_LOG(LogTemp, Error, TEXT("%s() No Owner"), *FString(__FUNCTION__));
        return Super::GetMaxSpeed();
    }

    if (!Owner->IsAlive())
    {
        return 0.0f;
    }
    if (Owner->GetAbilitySystemComponent()->HasMatchingGameplayTag(
        FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun"))))
    {
        return 0.0f;
    }

    if (RequestToStartSprinting)
    {
        return Owner->GetMoveSpeed() * SprintSpeedMultiplier;
    }

    if (RequestToStartADS)
    {
        return Owner->GetMoveSpeed() * ADSSpeedMultiplier;
    }

    return Owner->GetMoveSpeed();
}

void UGTCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
    Super::UpdateFromCompressedFlags(Flags);

    RequestToStartSprinting = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;

    RequestToStartADS = (Flags & FSavedMove_Character::FLAG_Custom_1) != 0;
}

FNetworkPredictionData_Client *UGTCharacterMovementComponent::GetPredictionData_Client() const
{
    //初始化并返回客户但预测数据的对象指针
    check(PawnOwner != NULL);

    if (!ClientPredictionData)
    {
        UGTCharacterMovementComponent *MutableThis = const_cast<UGTCharacterMovementComponent *>(this);

        /*
         *MaxClientSmoothingDeltaTime 最大的客户端平滑时间差（单位毫秒），用来控制客户端预测时的最大延迟
         *NoSmoothNetUpdateDist 不平滑网络更新的距离阈值，超过这个距离则不会进行平滑处理
         */
        MutableThis->ClientPredictionData                              = new FGTNetworkPredictionData_Client(*this);
        MutableThis->ClientPredictionData->MaxClientSmoothingDeltaTime = 92.0f;
        MutableThis->ClientPredictionData->NoSmoothNetUpdateDist       = 140.f;
    }

    return ClientPredictionData;
}

void UGTCharacterMovementComponent::StartSprinting()
{
    RequestToStartSprinting = true;
}

void UGTCharacterMovementComponent::StopSprinting()
{
    RequestToStartSprinting = false;
}

void UGTCharacterMovementComponent::StartAimDownSights()
{
    RequestToStartADS = true;
}

void UGTCharacterMovementComponent::StopAimDownSights()
{
    RequestToStartADS = false;
}

void UGTCharacterMovementComponent::FGTSavedMove::Clear()
{
    Super::Clear();

    SavedRequestToStartSprinting = false;
    SavedRequestToStartADS       = false;
}

//将一些特定的状态标识压缩到一个uint8类型变量中
uint8 UGTCharacterMovementComponent::FGTSavedMove::GetCompressedFlags() const
{
    uint8 Result = Super::GetCompressedFlags();
    if (SavedRequestToStartSprinting)
    {
        Result |= FLAG_Custom_0;
    }
    if (SavedRequestToStartADS)
    {
        Result |= FLAG_Custom_1;
    }

    return Result;
}

bool UGTCharacterMovementComponent::FGTSavedMove::CanCombineWith(const FSavedMovePtr &NewMove, ACharacter *InCharacter,
                                                                 float                MaxDelta) const
{
    if (SavedRequestToStartSprinting != ((FGTSavedMove *)&NewMove)->SavedRequestToStartSprinting)
    {
        return false;
    }

    if (SavedRequestToStartADS != ((FGTSavedMove *)&NewMove)->SavedRequestToStartADS)
    {
        return false;
    }

    return Super::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

void UGTCharacterMovementComponent::FGTSavedMove::SetMoveFor(ACharacter *C, float InDeltaTime, FVector const &NewAccel,
                                                             FNetworkPredictionData_Client_Character &ClientData)
{
    Super::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);

    UGTCharacterMovementComponent *CharacterMovement = Cast<UGTCharacterMovementComponent>(C->GetCharacterMovement());
    if (CharacterMovement)
    {
        SavedRequestToStartSprinting = CharacterMovement->RequestToStartSprinting;
        SavedRequestToStartADS       = CharacterMovement->RequestToStartADS;
    }
}

void UGTCharacterMovementComponent::FGTSavedMove::PrepMoveFor(ACharacter *C)
{
    Super::PrepMoveFor(C);

    UGTCharacterMovementComponent* CharacterMovement = Cast<UGTCharacterMovementComponent>(C->GetCharacterMovement());
    if(CharacterMovement)
    {
        
    }
}

UGTCharacterMovementComponent::FGTNetworkPredictionData_Client::FGTNetworkPredictionData_Client(const UCharacterMovementComponent &ClientMovement)
    :Super(ClientMovement)
{
    
}

FSavedMovePtr UGTCharacterMovementComponent::FGTNetworkPredictionData_Client::AllocateNewMove()
{
    return FSavedMovePtr(new FGTSavedMove());
}


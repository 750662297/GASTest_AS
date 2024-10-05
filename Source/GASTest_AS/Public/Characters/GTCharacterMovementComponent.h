// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GTCharacterMovementComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GASTEST_AS_API UGTCharacterMovementComponent
    : public UCharacterMovementComponent
{
    GENERATED_BODY()

    class FGTSavedMove : public FSavedMove_Character
    {
    public:
        typedef FSavedMove_Character Super;

        virtual void Clear() override;

        //将输入命令存储在压缩标志中
        virtual uint8 GetCompressedFlags() const override;

        //用来检查两个动作是否可以合并成一个
        virtual bool CanCombineWith(const FSavedMovePtr &NewMove, ACharacter *InCharacter, float MaxDelta) const override;

        //在将移动发送到服务器之前对其进行设置
        virtual void SetMoveFor(ACharacter *C, float InDeltaTime, FVector const &NewAccel, FNetworkPredictionData_Client_Character &ClientData) override;
        //在进行预测修正之前设置角色移动组件的变量
        virtual void PrepMoveFor(ACharacter *C) override;

        uint8 SavedRequestToStartSprinting : 1; //冲刺标识位

        uint8 SavedRequestToStartADS : 1; //ADS请求状态(Aim Down Sights
    };

    //处理客户端预测数据
    class FGTNetworkPredictionData_Client : public FNetworkPredictionData_Client_Character
    {
    public :
        FGTNetworkPredictionData_Client(const UCharacterMovementComponent& ClientMovement);

        typedef FNetworkPredictionData_Client_Character Super;

        //重写AllocateNewMove，每当需要一个新的移动记录时，都会使用包含额外状态信息的自定义类型
        virtual FSavedMovePtr AllocateNewMove() override;
    };

public:
    // Sets default values for this component's properties
    UGTCharacterMovementComponent();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprint")
    float SprintSpeedMultiplier;

    UPROPERTY(EditAnywhere,BlueprintReadWrite, Category =  "Aim Down Sights")
    float ADSSpeedMultiplier;

    uint8 RequestToStartSprinting : 1;
    uint8 RequestToStartADS : 1;

    virtual float GetMaxSpeed() const override;


    //用于从压缩后的标志位更新移动组件的状态，对于处理网络传输的数据有用
    virtual void UpdateFromCompressedFlags(uint8 Flags) override;
    //返回一个FNetworkPredictionData_Client指针，使客户端可以使用自定义的预测数据结构来进行网络同步
    virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;

    //Sprint
    UFUNCTION(BlueprintCallable, Category = "Sprint")
    void StartSprinting();

    UFUNCTION(BlueprintCallable, Category = "Sprint")
    void StopSprinting();

    //Aim Down Sights
    UFUNCTION(BlueprintCallable, Category = "Aim Down Sights")
    void StartAimDownSights();
    UFUNCTION(BlueprintCallable, Category = "Aim Down Sights")
    void StopAimDownSights();
};

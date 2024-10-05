#pragma once

#include "CoreMinimal.h"
#include "Characters/GTCharacterBase.h"
#include "GameFramework/PlayerController.h"
#include "GTPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GASTEST_AS_API AGTPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void CreateHUD();

	UPROPERTY(EditAnywhere, Category = "GASTest|UI")
	TSubclassOf<class UGTDamageTextWidgetComponent> DamageNumberClass;

	class UGTHUDWidget *GetHUD();

	/*
	 *Client 标记一个函数是否应该在网络上的客户端上调用。如果一个函数被标记为 Client，那么它将在接收到来自服务器的消息时，在每个客户端上执行。这类函数通常用于处理那些不需要影响服务器状态的操作，例如播放音效、视觉效果或者更新本地客户端的 UI 界面
	 *Reliable 定了网络消息是否应该可靠地发送。可靠消息保证了消息会按照发送的顺序到达，并且不会有丢失的风险。对于那些需要确保数据完整性和顺序的操作来说，使用 Reliable 是很重要的。例如，在射击游戏中，玩家的生命值减少就是一个可靠的事件，因为这直接影响到了游戏的状态
	 *Unreliable (与Reliable相反） 适用于丢失也不会导致严重后果的情况，比如粒子效果的触发等
	 *WithValidation  用于指定是否应该在函数执行前进行验证。验证通常用来检查函数调用的合法性，例如检查调用者的权限或者是当前状态是否允许执行该操作。如果验证失败，则函数不会被执行，并且可能会向调用者返回错误信息。这对于防止非法操作和保持游戏的一致性非常重要
	 */
	UFUNCTION(Client, Reliable, WithValidation)
	void ShowDamageNumber(float DamageAmount, AGTCharacterBase *TargetCharacter);
	void ShowDamageNumber_Implementation(float DamageAmount, AGTCharacterBase *TargetCharacter);
	bool ShowDamageNumber_Validate(float DamageAmount, AGTCharacterBase *TargetCharacter);

	UFUNCTION(Client, Reliable, WithValidation)
	void SetRespawnCountdown(float RespawnTimeRemaining);
	void SetRespawnCountdown_Implementation(float RespawnTimeRemaining);
	bool SetRespawnCountdown_Validate(float RespawnTimeRemaining);

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GASTest|UI")
	TSubclassOf<class UGTHUDWidget> UIHUDWidgetClass;

	UPROPERTY(BlueprintReadWrite, Category = "GASTest|UI")
	class UGTHUDWidget* UIHUDWidget;

	//only Server
	virtual void OnPossess(APawn *InPawn) override;

	virtual void OnRep_PlayerState() override;
};

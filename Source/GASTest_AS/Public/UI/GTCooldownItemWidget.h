// Copyright 2023 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "GTCooldownItemData.h"
#include "GTCooldownItemWidget.generated.h"

/**
 * 
 */
UCLASS()
class GASTEST_AS_API UGTCooldownItemWidget : public UUserWidget, public  IUserObjectListEntry
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void UpdateItemInfo(const UGTCooldownItemData* Info);

	UFUNCTION(BlueprintCallable)
	void UpdateBufferNumber(int32 Number);

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag ItemTag;
	
protected:
	virtual void NativeConstruct() override;

	

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere,meta =(BindWidget))
	class UTextBlock* TextName;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere,meta =(BindWidget))
	class UTextBlock* TextTime;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere,meta =(BindWidget))
	class UTextBlock* TextBufferNumber;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere,meta =(BindWidget))
	class UProgressBar* ProgressBar;

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
};

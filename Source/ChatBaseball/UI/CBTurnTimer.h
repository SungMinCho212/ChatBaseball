// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CBTurnTimer.generated.h"

class UTextBlock;
class ACBPlayerState;

UCLASS()
class CHATBASEBALL_API UCBTurnTimer : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable, Category = "TurnTimer")
	void UpdateTimerDisplay();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Timer;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_CurrentPlayer;

private:
	FTimerHandle UpdateTimerHandle;
};

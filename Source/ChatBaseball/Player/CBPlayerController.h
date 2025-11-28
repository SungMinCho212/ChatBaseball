// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CBPlayerController.generated.h"


class UCBChatInput;
class UCBTurnTimer;

UCLASS()
class CHATBASEBALL_API ACBPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	void SetChatMessageString(const FString& InChatMessageString);

	void PrintChatMessageString(const FString& InChatMessageString);

	UFUNCTION(Client, Reliable)
	void ClientRPCPrintChatMessageString(const FString& InChatMessageString);

	UFUNCTION(Server, Reliable)
	void ServerRPCProcessChatMessage(const FString& InChatMessageString);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UCBChatInput> ChatInputWidgetClass;

	UPROPERTY()
	TObjectPtr<UCBChatInput> ChatInputWidgetInstance;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UCBTurnTimer> TurnTimerWidgetClass;

	UPROPERTY()
	TObjectPtr<UCBTurnTimer> TurnTimerWidgetInstance;

	FString ChatMessageString;

};

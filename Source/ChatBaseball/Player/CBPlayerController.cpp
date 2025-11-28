// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CBPlayerController.h"
#include "UI/CBChatInput.h"
#include "UI/CBTurnTimer.h"
#include "Game/CBGameModeBase.h"
#include "Kismet/KismetSystemLibrary.h"


void ACBPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() == false)
	{
		return;
	}



	FInputModeUIOnly InputModeUIOnly;
	SetInputMode(InputModeUIOnly);

	if (IsValid(ChatInputWidgetClass) == true)
	{
		ChatInputWidgetInstance = CreateWidget<UCBChatInput>(this, ChatInputWidgetClass);
		if (IsValid(ChatInputWidgetInstance) == true)
		{
			ChatInputWidgetInstance->AddToViewport();
		}
	}

	// 턴 타이머 위젯 생성
	if (IsValid(TurnTimerWidgetClass) == true)
	{
		TurnTimerWidgetInstance = CreateWidget<UCBTurnTimer>(this, TurnTimerWidgetClass);
		if (IsValid(TurnTimerWidgetInstance) == true)
		{
			TurnTimerWidgetInstance->AddToViewport();
		}
	}

}

void ACBPlayerController::SetChatMessageString(const FString& InChatMessageString)
{
	ChatMessageString = InChatMessageString;

	// 서버로 메시지 전송
	ServerRPCProcessChatMessage(ChatMessageString);
}

void ACBPlayerController::PrintChatMessageString(const FString& InChatMessageString)
{
	UKismetSystemLibrary::PrintString(this, InChatMessageString, true, true, FLinearColor::Red, 5.0f);
}

void ACBPlayerController::ClientRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	PrintChatMessageString(InChatMessageString);
}

void ACBPlayerController::ServerRPCProcessChatMessage_Implementation(const FString& InChatMessageString)
{
	// 서버에서 GameMode의 ProcessChatMessage 호출
	AGameModeBase* GameMode = GetWorld()->GetAuthGameMode();
	if (IsValid(GameMode))
	{
		ACBGameModeBase* CBGameMode = Cast<ACBGameModeBase>(GameMode);
		if (IsValid(CBGameMode))
		{
			CBGameMode->ProcessChatMessage(this, InChatMessageString);
		}
	}
}

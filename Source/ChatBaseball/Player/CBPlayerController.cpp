// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CBPlayerController.h"
#include "UI/CBChatInput.h"
#include "UI/CBTurnTimer.h"
#include "Game/CBGameModeBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Blueprint/UserWidget.h"


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

	// Notification 위젯 생성
	if (IsValid(NotificationTextWidgetClass) == true)
	{
		NotificationTextWidgetInstance = CreateWidget<UUserWidget>(this, NotificationTextWidgetClass);
		if (IsValid(NotificationTextWidgetInstance) == true)
		{
			NotificationTextWidgetInstance->AddToViewport();
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

bool ACBPlayerController::ServerRPCProcessChatMessage_Validate(const FString& InChatMessageString)
{
	if (InChatMessageString.Len() > 100)
	{
		return false;
	}

	if (InChatMessageString.IsEmpty())
	{
		return false;
	}

	return true;
}

void ACBPlayerController::ServerRPCProcessChatMessage_Implementation(const FString& InChatMessageString)
{
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

void ACBPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACBPlayerController, NotificationText);
}

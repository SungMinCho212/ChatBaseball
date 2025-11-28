// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CBTurnTimer.h"
#include "Components/TextBlock.h"
#include "Player/CBPlayerState.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

void UCBTurnTimer::NativeConstruct()
{
	Super::NativeConstruct();

	UpdateTimerDisplay();

	GetWorld()->GetTimerManager().SetTimer(UpdateTimerHandle, this, &UCBTurnTimer::UpdateTimerDisplay, 0.1f, true);
}

void UCBTurnTimer::NativeDestruct()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(UpdateTimerHandle);
	}

	Super::NativeDestruct();
}

void UCBTurnTimer::UpdateTimerDisplay()
{
	TArray<AActor*> PlayerStates;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACBPlayerState::StaticClass(), PlayerStates);

	for (AActor* Actor : PlayerStates)
	{
		ACBPlayerState* PS = Cast<ACBPlayerState>(Actor);
		if (IsValid(PS) && PS->GetIsMyTurn())
		{
			float RemainingTime = PS->GetRemainingTurnTime();
			int32 Seconds = FMath::CeilToInt(RemainingTime);

			if (IsValid(TextBlock_Timer))
			{
				FString TimeText = FString::Printf(TEXT("남은 시간: %d초"), Seconds);
				TextBlock_Timer->SetText(FText::FromString(TimeText));

				if (Seconds <= 5)
				{
					TextBlock_Timer->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
				}
				else
				{
					TextBlock_Timer->SetColorAndOpacity(FSlateColor(FLinearColor::White));
				}
			}

			if (IsValid(TextBlock_CurrentPlayer))
			{
				FString PlayerName = PS->GetPlayerName();
				FString PlayerText = FString::Printf(TEXT("현재 턴: %s"), *PlayerName);
				TextBlock_CurrentPlayer->SetText(FText::FromString(PlayerText));
			}

			return;
		}
	}

	if (IsValid(TextBlock_Timer))
	{
		TextBlock_Timer->SetText(FText::FromString(TEXT("턴 대기 중...")));
		TextBlock_Timer->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	}

	if (IsValid(TextBlock_CurrentPlayer))
	{
		TextBlock_CurrentPlayer->SetText(FText::FromString(TEXT("현재 턴: -")));
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CBTurnTimer.h"
#include "Components/TextBlock.h"
#include "Player/CBPlayerState.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

void UCBTurnTimer::NativeConstruct()
{
	Super::NativeConstruct();

	UpdateTimerDisplay();
}

void UCBTurnTimer::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateTimerDisplay();
}

void UCBTurnTimer::UpdateTimerDisplay()
{
	// 모든 플레이어 스테이트 중에서 현재 턴인 플레이어 찾기
	TArray<AActor*> PlayerStates;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACBPlayerState::StaticClass(), PlayerStates);

	for (AActor* Actor : PlayerStates)
	{
		ACBPlayerState* PS = Cast<ACBPlayerState>(Actor);
		if (IsValid(PS) && PS->GetIsMyTurn())
		{
			// 남은 시간 표시
			float RemainingTime = PS->GetRemainingTurnTime();
			int32 Seconds = FMath::CeilToInt(RemainingTime);

			if (IsValid(TextBlock_Timer))
			{
				FString TimeText = FString::Printf(TEXT("남은 시간: %d초"), Seconds);
				TextBlock_Timer->SetText(FText::FromString(TimeText));

				// 시간이 5초 이하면 빨간색으로 표시
				if (Seconds <= 5)
				{
					TextBlock_Timer->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
				}
				else
				{
					TextBlock_Timer->SetColorAndOpacity(FSlateColor(FLinearColor::White));
				}
			}

			// 현재 턴 플레이어 이름 표시
			if (IsValid(TextBlock_CurrentPlayer))
			{
				FString PlayerName = PS->GetPlayerName();
				FString PlayerText = FString::Printf(TEXT("현재 턴: %s"), *PlayerName);
				TextBlock_CurrentPlayer->SetText(FText::FromString(PlayerText));
			}

			return;
		}
	}

	// 현재 턴인 플레이어가 없으면 대기 메시지 표시
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

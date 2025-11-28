// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/CBGameModeBase.h"
#include "Player/CBPlayerState.h"
#include "Player/CBPlayerController.h"
#include "Kismet/GameplayStatics.h"

ACBGameModeBase::ACBGameModeBase()
{
	PlayerStateClass = ACBPlayerState::StaticClass();
	bIsGameActive = true;
	CurrentTurnPlayerIndex = 0;
	TurnTimeLimit = 30.0f;
}

void ACBGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	GenerateAnswerNumber();

	FTimerHandle InitTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(InitTimerHandle, [this]()
	{
		PlayerList.Empty();
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			APlayerController* PC = It->Get();
			if (IsValid(PC))
			{
				PlayerList.Add(PC);
				UE_LOG(LogTemp, Warning, TEXT("[Server] Added player to list: %s"), *PC->GetPlayerState<APlayerState>()->GetPlayerName());
			}
		}

		UE_LOG(LogTemp, Warning, TEXT("[Server] Total players: %d"), PlayerList.Num());

		if (PlayerList.Num() > 0)
		{
			StartNextTurn();
		}
	}, 2.0f, false);
	
}

void ACBGameModeBase::GenerateAnswerNumber()
{
	TArray<int32> Numbers;

	for (int32 i = 1; i <= 9; i++)
	{
		Numbers.Add(i);
	}

	for (int32 i = Numbers.Num() - 1; i > 0; i--)
	{
		int32 RandomIndex = FMath::RandRange(0, i);
		Numbers.Swap(i, RandomIndex);
	}

	AnswerNumber = FString::Printf(TEXT("%d%d%d"), Numbers[0], Numbers[1], Numbers[2]);

	UE_LOG(LogTemp, Warning, TEXT("Answer Number Generated: %s"), *AnswerNumber);
}

bool ACBGameModeBase::IsValidNumber(const FString& Number) const
{
	if (Number.Len() != 3)
	{
		return false;
	}

	TArray<TCHAR> Digits;

	for (int32 i = 0; i < Number.Len(); i++)
	{
		TCHAR Char = Number[i];

		if (!FChar::IsDigit(Char))
		{
			return false;
		}

		if (Char == '0')
		{
			return false;
		}

		if (Digits.Contains(Char))
		{
			return false;
		}

		Digits.Add(Char);
	}

	return true;
}

FString ACBGameModeBase::CalculateResult(const FString& GuessNumber) const
{
	int32 Strikes = 0;
	int32 Balls = 0;

	for (int32 i = 0; i < 3; i++)
	{
		if (GuessNumber[i] == AnswerNumber[i])
		{
			Strikes++;
		}
		else if (AnswerNumber.Contains(FString::Chr(GuessNumber[i])))
		{
			Balls++;
		}
	}

	if (Strikes == 0 && Balls == 0)
	{
		return TEXT("OUT");
	}
	else
	{
		return FString::Printf(TEXT("%dS%dB"), Strikes, Balls);
	}
}

void ACBGameModeBase::ProcessChatMessage(APlayerController* PlayerController, const FString& Message)
{
	if (!bIsGameActive)
	{
		return;
	}

	ACBPlayerState* CBPlayerState = PlayerController->GetPlayerState<ACBPlayerState>();
	if (!IsValid(CBPlayerState))
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[Server] ProcessChatMessage from %s, IsMyTurn: %s"),
		*PlayerController->GetPlayerState<APlayerState>()->GetPlayerName(),
		CBPlayerState->GetIsMyTurn() ? TEXT("TRUE") : TEXT("FALSE"));

	if (!CBPlayerState->GetIsMyTurn())
	{
		BroadcastMessage(FString::Printf(TEXT("[%s] 자신의 턴이 아닙니다."), *PlayerController->GetPlayerState<APlayerState>()->GetPlayerName()));
		return;
	}

	if (CBPlayerState->GetRemainingTurnTime() <= 0.0f)
	{
		BroadcastMessage(FString::Printf(TEXT("[%s] 턴 시간이 종료되었습니다."), *PlayerController->GetPlayerState<APlayerState>()->GetPlayerName()));
		return;
	}

	if (CBPlayerState->GetHasWon() || CBPlayerState->GetCurrentAttempts() >= CBPlayerState->GetMaxAttempts())
	{
		BroadcastMessage(FString::Printf(TEXT("[%s] 더 이상 시도할 수 없습니다."), *PlayerController->GetPlayerState<APlayerState>()->GetPlayerName()));
		return;
	}

	if (!IsValidNumber(Message))
	{
		BroadcastMessage(FString::Printf(TEXT("[%s] 유효하지 않은 숫자입니다. (1~9 사이의 중복되지 않는 3자리 숫자를 입력하세요)"), *PlayerController->GetPlayerState<APlayerState>()->GetPlayerName()));
		return;
	}

	CBPlayerState->IncrementAttempts();

	CBPlayerState->SetHasTriedThisTurn(true);

	FString Result = CalculateResult(Message);

	FString BroadcastText = FString::Printf(TEXT("[%s] %s -> %s (%d/%d)"),
		*PlayerController->GetPlayerState<APlayerState>()->GetPlayerName(),
		*Message,
		*Result,
		CBPlayerState->GetCurrentAttempts(),
		CBPlayerState->GetMaxAttempts());

	BroadcastMessage(BroadcastText);

	CheckGameResult(PlayerController, Result);

	if (bIsGameActive)
	{
		EndCurrentTurn();
	}
}

void ACBGameModeBase::CheckGameResult(APlayerController* PlayerController, const FString& Result)
{
	ACBPlayerState* CBPlayerState = PlayerController->GetPlayerState<ACBPlayerState>();
	if (!IsValid(CBPlayerState))
	{
		return;
	}

	if (Result == TEXT("3S0B"))
	{
		CBPlayerState->SetHasWon(true);
		BroadcastMessage(FString::Printf(TEXT("========================================")));
		BroadcastMessage(FString::Printf(TEXT("🎉 승리! %s님이 정답을 맞췄습니다!"), *PlayerController->GetPlayerState<APlayerState>()->GetPlayerName()));
		BroadcastMessage(FString::Printf(TEXT("정답: %s"), *AnswerNumber));
		BroadcastMessage(FString::Printf(TEXT("========================================")));

		bIsGameActive = false;

		FTimerHandle ResetTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(ResetTimerHandle, this, &ACBGameModeBase::ResetGame, 3.0f, false);
		return;
	}

	bool bAllPlayersFinished = true;
	TArray<AActor*> PlayerStates;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACBPlayerState::StaticClass(), PlayerStates);

	for (AActor* Actor : PlayerStates)
	{
		ACBPlayerState* PS = Cast<ACBPlayerState>(Actor);
		if (IsValid(PS))
		{
			if (!PS->GetHasWon() && PS->GetCurrentAttempts() < PS->GetMaxAttempts())
			{
				bAllPlayersFinished = false;
				break;
			}
		}
	}

	if (bAllPlayersFinished)
	{
		bool bHasWinner = false;
		for (AActor* Actor : PlayerStates)
		{
			ACBPlayerState* PS = Cast<ACBPlayerState>(Actor);
			if (IsValid(PS) && PS->GetHasWon())
			{
				bHasWinner = true;
				break;
			}
		}

		if (!bHasWinner)
		{
			BroadcastMessage(FString::Printf(TEXT("========================================")));
			BroadcastMessage(FString::Printf(TEXT("무승부! 모든 플레이어가 시도 횟수를 소진했습니다.")));
			BroadcastMessage(FString::Printf(TEXT("정답: %s"), *AnswerNumber));
			BroadcastMessage(FString::Printf(TEXT("========================================")));

			bIsGameActive = false;

			FTimerHandle ResetTimerHandle;
			GetWorld()->GetTimerManager().SetTimer(ResetTimerHandle, this, &ACBGameModeBase::ResetGame, 3.0f, false);
		}
	}
}

void ACBGameModeBase::ResetGame()
{
	GetWorld()->GetTimerManager().ClearTimer(TurnTimerHandle);

	TArray<AActor*> PlayerStates;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACBPlayerState::StaticClass(), PlayerStates);

	for (AActor* Actor : PlayerStates)
	{
		ACBPlayerState* PS = Cast<ACBPlayerState>(Actor);
		if (IsValid(PS))
		{
			PS->ResetAttempts();
		}
	}

	GenerateAnswerNumber();

	CurrentTurnPlayerIndex = 0;

	bIsGameActive = true;

	BroadcastMessage(TEXT("게임이 리셋되었습니다! 새로운 게임을 시작합니다."));

	if (PlayerList.Num() > 0)
	{
		StartNextTurn();
	}
}

void ACBGameModeBase::BroadcastMessage(const FString& Message)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (IsValid(PC))
		{
			ACBPlayerController* CBPC = Cast<ACBPlayerController>(PC);
			if (IsValid(CBPC))
			{
				CBPC->ClientRPCPrintChatMessageString(Message);
			}
		}
	}
}

void ACBGameModeBase::StartNextTurn()
{
	UE_LOG(LogTemp, Warning, TEXT("[Server] StartNextTurn called. Active: %s, PlayerCount: %d"),
		bIsGameActive ? TEXT("TRUE") : TEXT("FALSE"), PlayerList.Num());

	if (!bIsGameActive || PlayerList.Num() == 0)
	{
		return;
	}

	GetWorld()->GetTimerManager().ClearTimer(TurnTimerHandle);

	int32 StartIndex = CurrentTurnPlayerIndex;
	bool bFoundNextPlayer = false;

	for (int32 i = 0; i < PlayerList.Num(); i++)
	{
		CurrentTurnPlayerIndex = (StartIndex + i) % PlayerList.Num();
		APlayerController* PC = PlayerList[CurrentTurnPlayerIndex];

		if (IsValid(PC))
		{
			ACBPlayerState* PS = PC->GetPlayerState<ACBPlayerState>();
			if (IsValid(PS) && !PS->GetHasWon() && PS->GetCurrentAttempts() < PS->GetMaxAttempts())
			{
				bFoundNextPlayer = true;
				break;
			}
		}
	}

	if (!bFoundNextPlayer)
	{
		CheckGameResult(nullptr, TEXT(""));
		return;
	}

	for (APlayerController* PC : PlayerList)
	{
		if (IsValid(PC))
		{
			ACBPlayerState* PS = PC->GetPlayerState<ACBPlayerState>();
			if (IsValid(PS))
			{
				PS->SetIsMyTurn(false);
				PS->SetRemainingTurnTime(0.0f);
			}
		}
	}

	APlayerController* CurrentPC = PlayerList[CurrentTurnPlayerIndex];
	if (IsValid(CurrentPC))
	{
		ACBPlayerState* CurrentPS = CurrentPC->GetPlayerState<ACBPlayerState>();
		if (IsValid(CurrentPS))
		{
			CurrentPS->SetIsMyTurn(true);
			CurrentPS->SetRemainingTurnTime(TurnTimeLimit);
			CurrentPS->SetHasTriedThisTurn(false);

			UE_LOG(LogTemp, Warning, TEXT("[Server] Setting turn for: %s"), *CurrentPC->GetPlayerState<APlayerState>()->GetPlayerName());

			BroadcastMessage(FString::Printf(TEXT("========================================")));
			BroadcastMessage(FString::Printf(TEXT("[%s]님의 턴입니다! (제한시간: %.0f초)"),
				*CurrentPC->GetPlayerState<APlayerState>()->GetPlayerName(),
				TurnTimeLimit));
			BroadcastMessage(FString::Printf(TEXT("========================================")));
		}
	}

	GetWorld()->GetTimerManager().SetTimer(TurnTimerHandle, this, &ACBGameModeBase::UpdateTurnTime, 0.1f, true);

	CurrentTurnPlayerIndex = (CurrentTurnPlayerIndex + 1) % PlayerList.Num();
}

void ACBGameModeBase::UpdateTurnTime()
{
	if (!bIsGameActive)
	{
		GetWorld()->GetTimerManager().ClearTimer(TurnTimerHandle);
		return;
	}

	ACBPlayerState* CurrentTurnPS = nullptr;
	APlayerController* CurrentTurnPC = nullptr;

	for (APlayerController* PC : PlayerList)
	{
		if (IsValid(PC))
		{
			ACBPlayerState* PS = PC->GetPlayerState<ACBPlayerState>();
			if (IsValid(PS) && PS->GetIsMyTurn())
			{
				CurrentTurnPS = PS;
				CurrentTurnPC = PC;
				break;
			}
		}
	}

	if (IsValid(CurrentTurnPS))
	{
		float NewTime = CurrentTurnPS->GetRemainingTurnTime() - 0.1f;
		CurrentTurnPS->SetRemainingTurnTime(FMath::Max(0.0f, NewTime));

		if (CurrentTurnPS->GetRemainingTurnTime() <= 0.0f)
		{
			if (!CurrentTurnPS->GetHasTriedThisTurn())
			{
				CurrentTurnPS->IncrementAttempts();
				BroadcastMessage(FString::Printf(TEXT("[%s] 턴 시간 종료! 기회가 소진되었습니다. (%d/%d)"),
					*CurrentTurnPC->GetPlayerState<APlayerState>()->GetPlayerName(),
					CurrentTurnPS->GetCurrentAttempts(),
					CurrentTurnPS->GetMaxAttempts()));
			}

			EndCurrentTurn();
		}
	}
}

void ACBGameModeBase::EndCurrentTurn()
{
	GetWorld()->GetTimerManager().ClearTimer(TurnTimerHandle);

	FTimerHandle NextTurnTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(NextTurnTimerHandle, this, &ACBGameModeBase::StartNextTurn, 0.5f, false);
}

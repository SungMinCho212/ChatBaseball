// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CBPlayerState.h"
#include "Net/UnrealNetwork.h"

ACBPlayerState::ACBPlayerState()
{
	CurrentAttempts = 0;
	MaxAttempts = 3;
	bHasWon = false;
	RemainingTurnTime = 0.0f;
	bIsMyTurn = false;
	bHasTriedThisTurn = false;
}

void ACBPlayerState::IncrementAttempts()
{
	if (CurrentAttempts < MaxAttempts)
	{
		CurrentAttempts++;
	}
}

void ACBPlayerState::ResetAttempts()
{
	CurrentAttempts = 0;
	bHasWon = false;
	bIsMyTurn = false;
	bHasTriedThisTurn = false;
	RemainingTurnTime = 0.0f;
}

void ACBPlayerState::OnRep_IsMyTurn()
{
	UE_LOG(LogTemp, Warning, TEXT("[%s] OnRep_IsMyTurn: %s"), *GetPlayerName(), bIsMyTurn ? TEXT("TRUE") : TEXT("FALSE"));
}

FString ACBPlayerState::GetPlayerInfoString() const
{
	FString PlayerInfoString = GetPlayerName() + TEXT("(") + FString::FromInt(CurrentAttempts) + TEXT("/") + FString::FromInt(MaxAttempts) + TEXT(")");
	return PlayerInfoString;
}

void ACBPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACBPlayerState, CurrentAttempts);
	DOREPLIFETIME(ACBPlayerState, bHasWon);
	DOREPLIFETIME(ACBPlayerState, RemainingTurnTime);
	DOREPLIFETIME(ACBPlayerState, bIsMyTurn);
}

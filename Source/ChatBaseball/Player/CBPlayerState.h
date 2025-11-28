// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CBPlayerState.generated.h"

UCLASS()
class CHATBASEBALL_API ACBPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ACBPlayerState();

	UFUNCTION(BlueprintCallable, Category = "Baseball")
	int32 GetCurrentAttempts() const { return CurrentAttempts; }

	UFUNCTION(BlueprintCallable, Category = "Baseball")
	int32 GetMaxAttempts() const { return MaxAttempts; }

	UFUNCTION(BlueprintCallable, Category = "Baseball")
	void IncrementAttempts();

	UFUNCTION(BlueprintCallable, Category = "Baseball")
	void ResetAttempts();

	UFUNCTION(BlueprintCallable, Category = "Baseball")
	void SetHasWon(bool bInHasWon) { bHasWon = bInHasWon; }

	UFUNCTION(BlueprintCallable, Category = "Baseball")
	bool GetHasWon() const { return bHasWon; }

	UFUNCTION(BlueprintCallable, Category = "Baseball")
	float GetRemainingTurnTime() const { return RemainingTurnTime; }

	UFUNCTION(BlueprintCallable, Category = "Baseball")
	void SetRemainingTurnTime(float InTime) { RemainingTurnTime = InTime; }

	UFUNCTION(BlueprintCallable, Category = "Baseball")
	bool GetIsMyTurn() const { return bIsMyTurn; }

	UFUNCTION(BlueprintCallable, Category = "Baseball")
	void SetIsMyTurn(bool bInIsMyTurn) { bIsMyTurn = bInIsMyTurn; }

	UFUNCTION(BlueprintCallable, Category = "Baseball")
	bool GetHasTriedThisTurn() const { return bHasTriedThisTurn; }

	UFUNCTION(BlueprintCallable, Category = "Baseball")
	void SetHasTriedThisTurn(bool bInHasTried) { bHasTriedThisTurn = bInHasTried; }

	UFUNCTION(BlueprintCallable, Category = "Baseball")
	FString GetPlayerInfoString() const;

protected:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Baseball")
	int32 CurrentAttempts;

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "Baseball")
	int32 MaxAttempts;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Baseball")
	bool bHasWon;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Baseball")
	float RemainingTurnTime;

	UPROPERTY(ReplicatedUsing = OnRep_IsMyTurn, VisibleAnywhere, BlueprintReadOnly, Category = "Baseball")
	bool bIsMyTurn;

	UFUNCTION()
	void OnRep_IsMyTurn();

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Baseball")
	bool bHasTriedThisTurn;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};

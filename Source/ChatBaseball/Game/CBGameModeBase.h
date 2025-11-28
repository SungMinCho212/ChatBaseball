// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CBGameModeBase.generated.h"

UCLASS()
class CHATBASEBALL_API ACBGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACBGameModeBase();

	virtual void BeginPlay() override;

	virtual void OnPostLogin(AController* NewPlayer) override;

	virtual void Logout(AController* Exiting) override;

	UFUNCTION(BlueprintCallable, Category = "Baseball")
	void ProcessChatMessage(APlayerController* PlayerController, const FString& Message);

protected:
	UFUNCTION(BlueprintCallable, Category = "Baseball")
	void GenerateAnswerNumber();

	UFUNCTION(BlueprintCallable, Category = "Baseball")
	bool IsValidNumber(const FString& Number) const;

	UFUNCTION(BlueprintCallable, Category = "Baseball")
	FString CalculateResult(const FString& GuessNumber) const;

	UFUNCTION(BlueprintCallable, Category = "Baseball")
	void CheckGameResult(APlayerController* PlayerController, const FString& Result);

	UFUNCTION(BlueprintCallable, Category = "Baseball")
	void ResetGame();

	UFUNCTION(BlueprintCallable, Category = "Baseball")
	void BroadcastMessage(const FString& Message);

	UFUNCTION(BlueprintCallable, Category = "Baseball")
	void StartNextTurn();

	UFUNCTION(BlueprintCallable, Category = "Baseball")
	void UpdateTurnTime();

	UFUNCTION(BlueprintCallable, Category = "Baseball")
	void EndCurrentTurn();

private:
	UPROPERTY()
	FString AnswerNumber;

	UPROPERTY()
	bool bIsGameActive;

	UPROPERTY()
	int32 CurrentTurnPlayerIndex;

	UPROPERTY(Config, EditDefaultsOnly, Category = "Baseball")
	float TurnTimeLimit;

	FTimerHandle TurnTimerHandle;

	FTimerHandle ResetTimerHandle;

	FTimerHandle NextTurnTimerHandle;

	UPROPERTY()
	TArray<APlayerController*> PlayerList;
};

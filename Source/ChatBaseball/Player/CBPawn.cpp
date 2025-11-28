// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CBPawn.h"
#include "ChatBaseball/ChatBaseball.h"


void ACBPawn::BeginPlay()
{
	Super::BeginPlay();

	FString NetModeString = ChatBaseballFunctionLibrary::GetRoleString(this);
	FString CombinedString = FString::Printf(TEXT("CBPawn::BeginPlay() %s [%s]"), *ChatBaseballFunctionLibrary::GetNetModeString(this), *NetModeString);
	ChatBaseballFunctionLibrary::MyPrintString(this, CombinedString, 10.f);
}

void ACBPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	FString NetModeString = ChatBaseballFunctionLibrary::GetRoleString(this);
	FString CombinedString = FString::Printf(TEXT("CBPawn::PossessedBy() %s [%s]"), *ChatBaseballFunctionLibrary::GetNetModeString(this), *NetModeString);
	ChatBaseballFunctionLibrary::MyPrintString(this, CombinedString, 10.f);
}

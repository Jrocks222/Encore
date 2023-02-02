// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Multiplayer5GameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER5_API ALobbyGameMode : public AMultiplayer5GameMode
{
	GENERATED_BODY()

public:
	ALobbyGameMode();

	UFUNCTION(BlueprintCallable)
		void ForceStart(FString DevKey);
	UFUNCTION(BlueprintImplementableEvent)
		void Travel();

	void PostLogin(APlayerController* NewPlayer) override;
	void Logout(AController* Exiting) override;

private:
	uint32 NumberOfPlayers = 0;
	FString Key = "131313";

};

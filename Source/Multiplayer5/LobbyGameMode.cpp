// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "Multiplayer5Character.h"
#include "EncoreHUD.h"
#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"

ALobbyGameMode::ALobbyGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Encore/InSession/Blueprints/EncoreCharacterBase"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	HUDClass = AEncoreHUD::StaticClass();
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer) {
	Super::PostLogin(NewPlayer);
	++NumberOfPlayers;

	if (NumberOfPlayers >= 10) {

		UWorld* World = GetWorld();
		if (!ensure(World != nullptr)) return;

		//Travel();
		//bUseSeamlessTravel = true;
		World->ServerTravel("/Game/maps/Monument/Monument?listen");
	}
}

void ALobbyGameMode::Logout(AController* Exiting) {
	Super::Logout(Exiting);
	--NumberOfPlayers;
}

void ALobbyGameMode::ForceStart(FString DevKey) {
	//if (DevKey != Key) return;
		UWorld* World = GetWorld();
		if (!ensure(World != nullptr)) return;

		//Travel();
		//bUseSeamlessTravel = true;
		World->ServerTravel("/Game/maps/Monument/Monument?listen");

}
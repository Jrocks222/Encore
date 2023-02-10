// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "OnlineSessionSettings.h"

#include "PlayFab.h"
#include "Core/PlayFabError.h"
#include "Core/PlayFabClientDataModels.h"
#include "Core/PlayFabServerDataModels.h"
#include "Core/PlayFabClientAPI.h"
#include "Core/PlayFabDataAPI.h"
#include "Core/PlayFabMultiplayerAPI.h"

#include "EOSTestingGameInstance.generated.h"

/**
 * 
 */

UCLASS()
class EOSTESTING_API UEOSTestingGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UEOSTestingGameInstance();

	UPROPERTY(BlueprintReadOnly)
		FString MatchmakingStatus = "Not Matchmaking";
	UPROPERTY(BlueprintReadWrite)
		UObject* CreatorData;

	virtual void Init() override;

	void Login();

	void OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);

	void PlayfabLogin();

	UFUNCTION(BlueprintNativeEvent)
		void OnConnectionComplete();

	UPROPERTY(BLueprintReadWrite)
		bool AllowMatchmake = true;

	UFUNCTION(BlueprintCallable)
		void BeginMatchmaking();

	UFUNCTION(Exec, BlueprintCallable)
		void TestServer();

	FString MatchmakingTicketId;

	FTimerHandle MatchmakingTimerHandle;

	UFUNCTION()
		void PollMatchmakingTicket(FString Id);

	void OnMatchMakingTicketResponse(const PlayFab::MultiplayerModels::FGetMatchmakingTicketResult& SuccessDelegate);

	FString MatchId;

	void GetMatch(FString MatchIdreq);

	void GetMatchSuccess(const PlayFab::MultiplayerModels::FGetMatchResult& SuccessDelegate);

	UFUNCTION(BlueprintCallable)
		void CreateSession();

	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

#if WITH_SERVER_CODE
	void Quit();
#endif


protected:
	class IOnlineSubsystem* OnlineSubsystem;

	PlayFab::ClientModels::FLoginResult PLoginResult;

private:
	PlayFabClientPtr clientAPI = nullptr;
	PlayFabMultiplayerPtr multiplayerAPI = nullptr;
	PlayFabDataPtr dataAPI = nullptr;
	PlayFabMatchmakerPtr matchmakingAPI = nullptr;

	
};

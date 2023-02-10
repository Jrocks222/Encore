// Fill out your copyright notice in the Description page of Project Settings.


#include "EOSTestingGameInstance.h"

#include "Core/PlayFabClientAPI.h"
#include "Core/PlayFabJsonHelpers.h"
#include <map>
#include <string>
#include "GenericPlatform/GenericPlatformMisc.h"
#include <iostream>
#include <cstdlib>
#include "PlayFabMultiplayerAPI.h"
#include "PlayFabMultiplayerModels.h"


UEOSTestingGameInstance::UEOSTestingGameInstance() {


}

void UEOSTestingGameInstance::Init() {

	Super::Init();

	OnlineSubsystem = IOnlineSubsystem::Get();
	Login();
}

void UEOSTestingGameInstance::Login() {

	if (OnlineSubsystem) {
		if (IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface()) {
			FOnlineAccountCredentials Credentials;
			Credentials.Id = FString();
			Credentials.Token = FString();
			Credentials.Type = FString("accountportal");
			Identity->OnLoginCompleteDelegates->AddUObject(this, &UEOSTestingGameInstance::OnLoginComplete);
			Identity->Login(0, Credentials);

		}
	}


}

void UEOSTestingGameInstance::OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error) {

	UE_LOG(LogTemp, Warning, TEXT("Login Success: %d"), bWasSuccessful)

		if (OnlineSubsystem) {
			if (IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface()) {
				Identity->ClearOnLoginCompleteDelegates(0, this);
			}
		}

	PlayfabLogin();
}

void UEOSTestingGameInstance::PlayfabLogin() {

	GetMutableDefault<UPlayFabRuntimeSettings>()->TitleId = TEXT("93248");

	clientAPI = IPlayFabModuleInterface::Get().GetClientAPI();
	multiplayerAPI = IPlayFabModuleInterface::Get().GetMultiplayerAPI();
	dataAPI = IPlayFabModuleInterface::Get().GetDataAPI();
	matchmakingAPI = IPlayFabModuleInterface::Get().GetMatchmakerAPI();

	PlayFab::ClientModels::FLoginWithOpenIdConnectRequest request;
	request.ConnectionId = TEXT("EOS");
	request.CreateAccount = true;
	if (OnlineSubsystem) {
		if (IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface()) {

			request.IdToken = Identity->GetAuthToken(0);

			clientAPI->LoginWithOpenIdConnect(request, 
				PlayFab::UPlayFabClientAPI::FLoginWithCustomIDDelegate::CreateLambda([&](const PlayFab::ClientModels::FLoginResult& Result) {
				UE_LOG(LogTemp, Warning, TEXT("PlayFab Login Successful"));
				
				PLoginResult = Result;
				
				OnConnectionComplete();

			}), 
				PlayFab::FPlayFabErrorDelegate::CreateLambda([](const PlayFab::FPlayFabCppError& ErrorResult) {
				UE_LOG(LogTemp, Warning, TEXT("Failed To Login To Playfab"));
			}));
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Failed To Retrieve Auth Token From Identity"));
		}
	}
}

void UEOSTestingGameInstance::OnConnectionComplete_Implementation()
{
	
}


void UEOSTestingGameInstance::BeginMatchmaking() {

	if (AllowMatchmake) {
		AllowMatchmake = false;

		TSharedPtr<FJsonObject> LatencyItem = MakeShared<FJsonObject>();
		LatencyItem->SetStringField(TEXT("region"), TEXT("EastUs"));
		LatencyItem->SetStringField(TEXT("latency"), TEXT("150"));

		TArray<TSharedPtr<FJsonValue> > LatencyItemArray;
		TSharedRef<FJsonValueObject> LatencyItemValue = MakeShareable(new FJsonValueObject(LatencyItem));
		LatencyItemArray.Add(LatencyItemValue);

		TSharedPtr<FJsonObject> LatenciesItem = MakeShareable(new FJsonObject);
		LatenciesItem->SetArrayField(TEXT("Latencies"), LatencyItemArray);

		TSharedPtr<FJsonObject> AttributesItem = MakeShareable(new FJsonObject);
		AttributesItem->SetObjectField(TEXT("DataObject"), LatenciesItem);

		TSharedPtr<FJsonObject> EntityItem = MakeShareable(new FJsonObject);
		EntityItem->SetStringField(TEXT("Id"), PLoginResult.EntityToken.Get()->Entity.Get()->Id);
		EntityItem->SetStringField(TEXT("Type"), PLoginResult.EntityToken.Get()->Entity.Get()->Type);

		TSharedPtr<FJsonObject> Creator = MakeShareable(new FJsonObject);
		Creator->SetObjectField(TEXT("Attributes"), AttributesItem);
		Creator->SetObjectField(TEXT("Entity"), EntityItem);



		PlayFab::MultiplayerModels::FCreateMatchmakingTicketRequest request;
		request.GiveUpAfterSeconds = 60;
		request.QueueName = TEXT("Testing");
		request.Creator = Creator;
		PlayFab::MultiplayerModels::FEntityKey;
		multiplayerAPI->CreateMatchmakingTicket(request,
			PlayFab::UPlayFabMultiplayerAPI::FCreateMatchmakingTicketDelegate::CreateLambda([this](const PlayFab::MultiplayerModels::FCreateMatchmakingTicketResult& Result) {

			UE_LOG(LogTemp, Warning, TEXT("Matchmaking Ticket Successfully Created: %s"), *FString(Result.TicketId));
		MatchmakingTicketId = Result.TicketId;

		}),
			PlayFab::FPlayFabErrorDelegate::CreateLambda([&](const PlayFab::FPlayFabCppError& Result) {

			UE_LOG(LogTemp, Warning, TEXT("Failed To Create Matchmaking Ticket %s"), *FString(Result.GenerateErrorReport()));
		AllowMatchmake = true;

		}));


		GetWorld()->GetTimerManager().SetTimer(MatchmakingTimerHandle, FTimerDelegate::CreateLambda([this]() {

			PollMatchmakingTicket(MatchmakingTicketId);

		}), 6, true);
	}

}

void UEOSTestingGameInstance::TestServer() {


	PlayFab::MultiplayerModels::FRequestMultiplayerServerRequest request;
	request.BuildId = TEXT("6c567f07-71bb-477d-948a-988a9ae13c36");
	FGuid SessionId = FGuid::NewGuid();
	request.SessionId = SessionId.ToString();

	TArray<FString> PreferredRegions;
	PreferredRegions.Add(TEXT("EastUS"));
	request.PreferredRegions = PreferredRegions;

	multiplayerAPI->RequestMultiplayerServer(request,
		PlayFab::UPlayFabMultiplayerAPI::FRequestMultiplayerServerDelegate::CreateLambda([&](const PlayFab::MultiplayerModels::FRequestMultiplayerServerResponse& SuccessDelegate) {

		FString Address;
		Address.Append(SuccessDelegate.IPV4Address);
		Address.Append(TEXT(":"));
		Address.Append(FString::FromInt(SuccessDelegate.Ports[0].Num));


		APlayerController* PlayerController = GetFirstLocalPlayerController();
		if (!ensure(PlayerController != nullptr)) return;

		PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);

	}),
		PlayFab::FPlayFabErrorDelegate::CreateLambda([&](const PlayFab::FPlayFabCppError& ErrorDelegate) {

		UE_LOG(LogTemp, Warning, TEXT("Failed To Get Multiplayer Server %s"), *FString(ErrorDelegate.GenerateErrorReport()));

	}));
}
	


void UEOSTestingGameInstance::PollMatchmakingTicket(FString Id) {

	PlayFab::MultiplayerModels::FGetMatchmakingTicketRequest request;
	request.TicketId = Id;
	request.QueueName = "Testing";
	multiplayerAPI->GetMatchmakingTicket(request, PlayFab::UPlayFabMultiplayerAPI::FGetMatchmakingTicketDelegate::CreateUObject(this, &UEOSTestingGameInstance::OnMatchMakingTicketResponse),
		PlayFab::FPlayFabErrorDelegate::CreateLambda([&](const PlayFab::FPlayFabCppError& Result) {

		UE_LOG(LogTemp, Warning, TEXT("Failed To Poll MatchmakingTicket %s"), *FString(Result.ErrorMessage));
	GetWorld()->GetTimerManager().ClearTimer(MatchmakingTimerHandle);
	AllowMatchmake = true;

	}));
}

void UEOSTestingGameInstance::OnMatchMakingTicketResponse(const PlayFab::MultiplayerModels::FGetMatchmakingTicketResult &SuccessDelegate) {

	MatchmakingStatus = SuccessDelegate.Status;
	UE_LOG(LogTemp, Warning, TEXT("MatchmakingStatus: %s"), *FString(MatchmakingStatus));

	if (SuccessDelegate.Status == "matched") {

		MatchId = SuccessDelegate.MatchId;
		MatchmakingStatus = SuccessDelegate.Status;
		GetWorld()->GetTimerManager().ClearTimer(MatchmakingTimerHandle);
		GetMatch(MatchId);
		AllowMatchmake = true;

	}
	else if (SuccessDelegate.Status == "Canceled") {
		GetWorld()->GetTimerManager().ClearTimer(MatchmakingTimerHandle);
		AllowMatchmake = true;

	}
}

void UEOSTestingGameInstance::GetMatch(FString MatchIdreq) {
	PlayFab::MultiplayerModels::FGetMatchRequest request;
	request.MatchId = MatchIdreq;
	request.QueueName = "Testing";
	multiplayerAPI->GetMatch(request, PlayFab::UPlayFabMultiplayerAPI::FGetMatchDelegate::CreateUObject(this, &UEOSTestingGameInstance::GetMatchSuccess),
		
		PlayFab::FPlayFabErrorDelegate::CreateLambda([&](const PlayFab::FPlayFabCppError& Result) {
	UE_LOG(LogTemp, Warning, TEXT("Failed To Poll MatchmakingTicket %s"), *FString(Result.ErrorMessage));
	AllowMatchmake = true;

	}));
}


void UEOSTestingGameInstance::GetMatchSuccess(const PlayFab::MultiplayerModels::FGetMatchResult& SuccessDelegate) {
	FString Address = "";
	Address.Append(SuccessDelegate.pfServerDetails.Get()->IPV4Address);
	Address.Append(":");
	Address.Append(FString::FromInt(SuccessDelegate.pfServerDetails.Get()->Ports[0].Num));

	PlayFab::MultiplayerModels::Destroy

	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
}

void UEOSTestingGameInstance::CreateSession() {

	if (OnlineSubsystem) {

		if (IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface()) {

			FOnlineSessionSettings SessionSettings;
			SessionSettings.bIsDedicated = false;
			SessionSettings.bShouldAdvertise = true;
			SessionSettings.bIsLANMatch = true;
			SessionSettings.NumPublicConnections = 10;
			SessionSettings.bUsesPresence = true;
			SessionSettings.bAllowJoinViaPresence = true;

			SessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &UEOSTestingGameInstance::OnCreateSessionComplete);
			SessionPtr->CreateSession(0, FName("Test Session"), SessionSettings);
		}
	}
}


void UEOSTestingGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful) {

	UE_LOG(LogTemp, Warning, TEXT("Success: %d"), bWasSuccessful)

		if (OnlineSubsystem) {
			if (IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface()) {
				SessionPtr->ClearOnCreateSessionCompleteDelegates(this);
			}
		}
}



void UEOSTestingGameInstance::Quit() {

	std::exit(0);
	FGenericPlatformMisc::RequestExit(false);
}

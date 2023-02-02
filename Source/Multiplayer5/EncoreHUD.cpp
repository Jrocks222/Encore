// Fill out your copyright notice in the Description page of Project Settings.


#include "EncoreHUD.h"
#include "SLoadingScreen.h"
#include "Widgets/SWeakWidget.h"
#include "Engine/Engine.h"

void AEncoreHUD::BeginPlay() {
	
	Super::BeginPlay();
	RemoveLoadingScreen();
}

void AEncoreHUD::ShowLoadingScreen() {
	if (GEngine && GEngine->GameViewport) {
		LoadingScreenWidget = SNew(SLoadingScreen).OwningHUD(this);
		GEngine->GameViewport->AddViewportWidgetContent(SAssignNew(MenuWidgetContainer, SWeakWidget).PossiblyNullContent(LoadingScreenWidget.ToSharedRef()), 10);
	}

	GEngine->AddOnScreenDebugMessage(0, 7, FColor::Red, FString::Printf(TEXT("Shown")));
}

void AEncoreHUD::RemoveLoadingScreen() {
	if (GEngine && MenuWidgetContainer.IsValid()) {
		GEngine->GameViewport->RemoveViewportWidgetContent(MenuWidgetContainer.ToSharedRef());
	}
}

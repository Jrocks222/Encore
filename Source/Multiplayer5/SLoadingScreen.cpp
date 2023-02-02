// Fill out your copyright notice in the Description page of Project Settings.


#include "SLoadingScreen.h"
//#include "MediaPlayer.h"

void SLoadingScreen::Construct(const FArguments& InArgs) {
	//UMediaPlayer* LoadingMedia = LoadObject<UMediaPlayer>(nullptr, TEXT("/Game/Movies/LoadingScreenMediaPlayer"));

	ChildSlot
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
			[
				SNew(SImage)
				//.ColorAndOpacity(FColor::Black)
				//.Image(new FSlateImageBrush(LoadingMedia, FVector2D(64.0f, 64.0f)))
				
			]
		];
}

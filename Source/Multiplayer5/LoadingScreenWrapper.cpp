// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadingScreenWrapper.h"
#include "SLoadingScreen.h"

TSharedRef<SWidget> ULoadingScreenWrapper::RebuildWidget() {
	return SNew(SLoadingScreen);
}


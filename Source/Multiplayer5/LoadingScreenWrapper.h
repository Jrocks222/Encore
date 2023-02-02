// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "LoadingScreenWrapper.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER5_API ULoadingScreenWrapper : public UWidget
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite, Category = "User Created")
	UUserWidget* LoadingScreenWrapper;
	
protected:

	virtual TSharedRef<SWidget> RebuildWidget() override;
};

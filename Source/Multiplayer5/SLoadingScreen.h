// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "SlateBasics.h"
#include "SlateExtras.h"

/**
 * 
 */
class SLoadingScreen : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SLoadingScreen) {}

	SLATE_ARGUMENT(TWeakObjectPtr<class AEncoreHUD>, OwningHUD)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	TWeakObjectPtr<class AEncoreHUD> OwningHUD;

	virtual bool SupportsKeyboardFocus() const override { return true; };
};

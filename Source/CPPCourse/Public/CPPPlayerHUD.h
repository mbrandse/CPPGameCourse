// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CPPPlayerHUD.generated.h"

/**
 * 
 */
UCLASS()
class CPPCOURSE_API UCPPPlayerHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UCPPPlayerHUD(const FObjectInitializer& ObjectInitializer);

	virtual void NativePreConstruct();

	class UCanvasPanel* RootPanel;

	class UImage* TestImage;
};

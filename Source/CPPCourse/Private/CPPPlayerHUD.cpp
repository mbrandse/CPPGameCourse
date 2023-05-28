// Fill out your copyright notice in the Description page of Project Settings.


#include "CPPPlayerHUD.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "CPPCourse/CPPCourse.h"

UCPPPlayerHUD::UCPPPlayerHUD(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{

}

void UCPPPlayerHUD::NativePreConstruct()
{
	Super::NativePreConstruct();

	RootPanel = Cast<UCanvasPanel>(GetRootWidget());
	if (!RootPanel)
		return;

	TestImage = NewObject<UImage>(this, UImage::StaticClass());
	UCanvasPanelSlot* TestSlot = RootPanel->AddChildToCanvas(TestImage);
	TestSlot->SetSize(FVector2D(100.f, 100.f));
	TestSlot->SetPosition(FVector2D(100.f, 100.f));
}
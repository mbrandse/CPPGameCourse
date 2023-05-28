// Fill out your copyright notice in the Description page of Project Settings.


#include "CPPUIEnemyHealth.h"
#include "CPPEnemyCharacter.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CapsuleComponent.h"
#include "CPPCourse/CPPCourse.h"


void UCPPUIEnemyHealth::NativePreConstruct()
{
	Super::NativePreConstruct();

	UCanvasPanel* RootPanel = Cast<UCanvasPanel>(GetRootWidget());
	if (!RootPanel) return;

	HealthPanel = NewObject<UCanvasPanel>(UCanvasPanel::StaticClass());
	UCanvasPanelSlot* HealthPanelSlot = RootPanel->AddChildToCanvas(HealthPanel);
	HealthPanelSlot->SetAutoSize(true);

	EnemyName = NewObject<UTextBlock>(UTextBlock::StaticClass());
	EnemyName->SetJustification(ETextJustify::Center);
	EnemyName->SetText(FText::FromString(TEXT("Enemy")));
	EnemyName->Font.Size = 20.f;
	EnemyName->Font.OutlineSettings.OutlineSize = 2.f;
	EnemyNameSlot = HealthPanel->AddChildToCanvas(EnemyName);
	EnemyNameSlot->SetZOrder(0);
	EnemyNameSlot->SetSize(FVector2D(200.f, 30.f));

	EnemyHealth = NewObject<UImage>(UImage::StaticClass());
	EnemyHealth->SetColorAndOpacity(FColor::Red);
	EnemyHealth->SetRenderTransformPivot(FVector2D(0.f, 0.5f));
	EnemyHealth->SetRenderScale(FVector2D(1.f, 1.f));
	EnemyHealthSlot = HealthPanel->AddChildToCanvas(EnemyHealth);
	EnemyHealthSlot->SetZOrder(1);
	EnemyHealthSlot->SetSize(FVector2D(194.f, 14.f));
	EnemyHealthSlot->SetPosition(FVector2D(3.f, 38.f));

	EnemyHealthBG = NewObject<UImage>(UImage::StaticClass());
	EnemyHealthBG->SetColorAndOpacity(FColor::Black);
	EnemyHealthBGSlot = HealthPanel->AddChildToCanvas(EnemyHealthBG);
	EnemyHealthBGSlot->SetZOrder(0);
	EnemyHealthBGSlot->SetSize(FVector2D(200.f, 20.f));
	EnemyHealthBGSlot->SetPosition(FVector2D(0.f, 35.f));
}

void UCPPUIEnemyHealth::SetEnemyOwner(ACPPEnemyCharacter* Enemy)
{
	if (Enemy)
	{
		EnemyOwner = Enemy;
		ICPPEnemyInterface* EInterface = Cast<ICPPEnemyInterface>(Enemy);
		
		if (EInterface)
		{
			//Set initial values as well, but be careful. At this point our elements may not exist yet. 	
			if(EnemyName) EnemyName->SetText(FText::FromString(EInterface->Execute_GetEnemyName(Enemy)));
			if(EnemyHealth) UpdateHealth();
		}
	}
}

void UCPPUIEnemyHealth::UpdateHealth()
{
	if (EnemyHealthSlot && EnemyOwner)
	{
		ICPPEnemyInterface* EInterface = Cast<ICPPEnemyInterface>(EnemyOwner);
		if (EInterface)
		{
			float CurrentHealth, MaxHealth;
			EInterface->Execute_GetHealth(EnemyOwner, MaxHealth, CurrentHealth);
			if (MaxHealth <= 0.f) return; //don't want division by 0 errors
			EnemyHealth->SetRenderScale(FVector2D(CurrentHealth / MaxHealth, 1.f));
		}
	}
}

//void UCPPUIEnemyHealth::ToggleVisibility(bool bNewVisibility)
//{
//	if (HealthPanel)
//	{
//		if (bNewVisibility)
//		{
//			HealthPanel->SetVisibility(ESlateVisibility::Visible);
//		}
//		else
//			HealthPanel->SetVisibility(ESlateVisibility::Hidden);
//	}
//}

void UCPPUIEnemyHealth::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (HealthPanel
		&& EnemyOwner
		&& GetWorld())
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (!PC) return;

		FVector WorldPosition = EnemyOwner->GetActorLocation();
		WorldPosition.Z += EnemyOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		FVector2D ScreenPos;

		//UGameplayStatics::ProjectWorldToScreen(PC, WorldPosition, ScreenPos);
		UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(PC, WorldPosition, ScreenPos, true);
		ScreenPos.X -= 100.f;
		ScreenPos.Y -= 100.f;
		HealthPanel->SetRenderTranslation(ScreenPos);
	}
}
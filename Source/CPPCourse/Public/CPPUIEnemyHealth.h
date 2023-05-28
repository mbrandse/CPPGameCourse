// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CPPUIEnemyHealth.generated.h"

/**
 * 
 */
UCLASS()
class CPPCOURSE_API UCPPUIEnemyHealth : public UUserWidget
{
	GENERATED_BODY()

public:

	void SetEnemyOwner(class ACPPEnemyCharacter* Enemy);

	virtual void NativePreConstruct();
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	class ACPPEnemyCharacter* EnemyOwner;

	class UCanvasPanel* HealthPanel;

	UPROPERTY()
		class UTextBlock* EnemyName;

	UPROPERTY()
		class UCanvasPanelSlot* EnemyNameSlot;

	UPROPERTY()
		class UImage* EnemyHealth;

	UPROPERTY()
		class UCanvasPanelSlot* EnemyHealthSlot;

	UPROPERTY()
		class UImage* EnemyHealthBG;

	UPROPERTY()
		class UCanvasPanelSlot* EnemyHealthBGSlot;

	UFUNCTION(BlueprintCallable, Category = "UI")
		void UpdateHealth();	

	//UFUNCTION(BlueprintCallable, Category = "UI")
	//	void ToggleVisibility(bool bNewVisibility);
};

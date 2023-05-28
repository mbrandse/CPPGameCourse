// Fill out your copyright notice in the Description page of Project Settings.


#include "CPPEnemyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "CPPEnemyInterface.h"
#include "CPPUtilityManager.h"
#include "CPPUIEnemyHealth.h"
#include "CPPCourse/CPPCourse.h"
#include "AIController.h"
#include "BrainComponent.h"

// Sets default values
ACPPEnemyCharacter::ACPPEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a utility manager
	UtilityManager = CreateDefaultSubobject<UCPPUtilityManager>(TEXT("Utility Manager"));
	//OnTakeAnyDamage.AddDynamic(this, &ACPPEnemyCharacter::OnAnyDamage);

	MaxHealth = 2.f;
}

// Called when the game starts or when spawned
void ACPPEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
	
	if (GetWorld()
		&& EnemyHealthUITemplate)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		EnemyHealthUI = CreateWidget<UCPPUIEnemyHealth>(PC, EnemyHealthUITemplate);
		EnemyHealthUI->AddToViewport(); //this needs to be called first so we can get the nativeconstruct call before we start setting owners.
		EnemyHealthUI->SetEnemyOwner(this);
		EnemyHealthUI->SetVisibility(ESlateVisibility::Hidden);
	}
}

// Called every frame
void ACPPEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ACPPEnemyCharacter::CanAttack()
{
	if (GetWorld())
	{
		TArray<AActor*> Enemies;
		UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UCPPEnemyInterface::StaticClass(), Enemies);
		for (AActor* SingleEnemy : Enemies)
		{
			//skip if enemy is current
			if (SingleEnemy == this)
				continue;

			ICPPEnemyInterface* EnemyInterface = Cast<ICPPEnemyInterface>(SingleEnemy);
			if (EnemyInterface
				&& !EnemyInterface->Execute_IsDead(SingleEnemy)
				&& EnemyInterface->Execute_IsAttacking(SingleEnemy))
				return false;
		}

		return true;
	}

	return false;
}

float ACPPEnemyCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	if (MaxHealth != 0.f
		&& CurrentHealth != 0.f
		&& GetWorld())
	{
		CurrentHealth -= Damage;
		if (CurrentHealth <= 0.f)
		{
			CurrentHealth = 0.f;
			OnDeath();
		}

		OnCancelDueToDamage.Broadcast(); //make sure we have this so we can cancel any BT related thingies.

		EnemyHealthUI->UpdateHealth();
		ReceiveTakeDamage(Damage, DamageEvent.DamageTypeClass, EventInstigator, DamageCauser);

		if (EnemyHealthUI->GetVisibility() == ESlateVisibility::Hidden)
		{
			EnemyHealthUI->SetVisibility(ESlateVisibility::Visible);
			GetWorld()->GetTimerManager().SetTimer(HideUITimer, this, &ACPPEnemyCharacter::HideHealthUI, 5.f, false);
		}
	}

	return Damage;
}

void ACPPEnemyCharacter::HideHealthUI()
{
	EnemyHealthUI->SetVisibility(ESlateVisibility::Hidden);
}

void ACPPEnemyCharacter::OnDeath()
{
	EnemyHealthUI->RemoveFromParent();
	OnEnemyDeath.Broadcast(this);

	Super::OnDeath();
}

bool ACPPEnemyCharacter::IsDead_Implementation()
{
	return CurrentHealth <= 0.f;
}

void ACPPEnemyCharacter::GetHealth_Implementation(float& InterfaceMaxHealth, float& InterfaceCurrentHealth)
{
	InterfaceMaxHealth = MaxHealth;
	InterfaceCurrentHealth = CurrentHealth;
}

void ACPPEnemyCharacter::PauseAIExecution(bool bNewPause)
{
	AAIController* AIC = Cast<AAIController>(GetController());
	if (AIC)
	{
		if (bNewPause)
		{
			AIC->GetBrainComponent()->StopLogic(TEXT("Taking a break"));
			GetUtilityManager()->PauseTaskExecution(true);
		}
		else
		{
			AIC->GetBrainComponent()->RestartLogic();
			GetUtilityManager()->PauseTaskExecution(false);
		}
	}
}
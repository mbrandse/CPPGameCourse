// Fill out your copyright notice in the Description page of Project Settings.

#include "CPPPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "CPPEnemyInterface.h"
#include "CPPCourse/CPPCourse.h"
#include "Camera/CameraActor.h"
#include "CPPFunctionLibrary.h"
#include "CPPEnemyCharacter.h"
#include "CPPUIEnemyHealth.h"
#include "CPPDamageBase.h"

ACPPPlayerCharacter::ACPPPlayerCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rate for input
	TurnRateGamepad = 50.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 1200.f;
	GetCharacterMovement()->GravityScale = 2.6f;
	GetCharacterMovement()->AirControl = 0.8f;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->bDoCollisionTest = true;
	CameraBoom->ProbeChannel = ECC_Visibility; //we set the default probe channel (boom collision test channel) to visibility, so we can ignore enemies, but not walls.

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	MaxStrafeSpeed = 300.f;
	MaxTargetingDistance = 2000.f;
	bIsTargeting = false;
	TargetingVector = FVector(0.f, 0.f, 1.f);
	TargetingIndex = 0;
	TargetCameraLocation = FVector(-400.f, 100.f, 100.f);
	bCanMove = true;

	MaxHealth = 20.f;
	bIsDead = false;
	DamageAnimationState = EDamageWeight::PDA_None;
	RequestedDamageRecoveryTime = 0.f;
}

void ACPPPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Setting realtime defaults here
	GetAllTargetingActors();

	//Get the target camera

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	TargetCamera = GetWorld()->SpawnActor<ACameraActor>(GetActorLocation(), GetActorRotation());
	if (TargetCamera)
	{
		TargetCamera->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		TargetCamera->SetActorRelativeLocation(TargetCameraLocation);
	}

	//bind damage function too
	OnTakePointDamage.AddDynamic(this, &ACPPPlayerCharacter::TakePointDamage);
	CurrentHealth = MaxHealth;
}

void ACPPPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//////////////////////////////////////////////////////////////////////////
// Input

void ACPPPlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACPPPlayerCharacter::StartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACPPPlayerCharacter::StopJump);
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &ACPPCourseCharacter::StartAttack);
	PlayerInputComponent->BindAction("Attack", IE_Released, this, &ACPPCourseCharacter::StopAttack);

	//Targeting
	PlayerInputComponent->BindAction("ToggleTargeting", IE_Released, this, &ACPPPlayerCharacter::ToggleTargeting);
	PlayerInputComponent->BindAction("NextTarget", IE_Released, this, &ACPPPlayerCharacter::GetNextTarget);
	PlayerInputComponent->BindAction("PreviousTarget", IE_Released, this, &ACPPPlayerCharacter::GetPreviousTarget);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &ACPPPlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &ACPPPlayerCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &ACPPPlayerCharacter::TurnAtRateMouse);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &ACPPPlayerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &ACPPPlayerCharacter::LookUpAtRateMouse);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &ACPPPlayerCharacter::LookUpAtRate);
}

void ACPPPlayerCharacter::StartJump()
{
	if (bCanMove)
		Jump();
}

void ACPPPlayerCharacter::StopJump()
{
	StopJumping();
}

void ACPPPlayerCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	if (!bIsTargeting)
		AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void ACPPPlayerCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	if (!bIsTargeting)
		AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void ACPPPlayerCharacter::TurnAtRateMouse(float Rate)
{
	if (GetWorld())
	{
		if (!bIsTargeting)
		{
			AddControllerYawInput(Rate);
			CameraBoom->TargetOffset = FVector::ZeroVector;
		}
		else
		{
			if (TargetCamera)
			{
				FRotator NewRot = FRotationMatrix::MakeFromX(TargetingActor->GetActorLocation() - TargetCamera->GetActorLocation()).Rotator();
				TargetCamera->SetActorRotation(NewRot);
				TargetCamera->SetActorRelativeLocation(TargetCameraLocation); //First make sure we trace the came from the very start

				FHitResult HitResult;
				FVector DesiredTargetCameraLocation;
				//Make sure to use the visibility channel here, so we can ignore any pawn related classes.
				bool bHit = GetWorld()->SweepSingleByChannel(HitResult, TargetingActor->GetActorLocation(), TargetCamera->GetActorLocation(), FQuat::Identity, GetCameraBoom()->ProbeChannel, FCollisionShape::MakeSphere(GetCameraBoom()->ProbeSize));
				if (bHit) //don't have to worry about camera, since that one doesnt have collision
				{
					//UE_LOG(AnyLog, Warning, TEXT("Current Hit Actor is %s"), *(HitResult.GetActor()->GetName()));
					DesiredTargetCameraLocation = GetActorTransform().InverseTransformPosition(HitResult.ImpactPoint);
				}
				else
				{
					DesiredTargetCameraLocation = TargetCameraLocation;
				}

				TargetCamera->SetActorRelativeLocation(DesiredTargetCameraLocation);
			}
		}
	}
}

void ACPPPlayerCharacter::LookUpAtRateMouse(float Rate)
{
	if (!bIsTargeting)
		AddControllerPitchInput(Rate);
}

void ACPPPlayerCharacter::GetAllTargetingActors()
{
	if (GetWorld())
	{
		TArray<AActor*> Actors;
		UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UCPPEnemyInterface::StaticClass(), Actors);
		//UE_LOG(AnyLog, Warning, TEXT("FOUND TARGETING ACTORS COUNT IS %d"), Actors.Num());
		for (AActor* Actor : Actors)
		{
			if (Actor)
			{
				TargetingActors.Add(Actor);
			}
		}
	}
}

TArray<AActor*> ACPPPlayerCharacter::GetAllTargetingActorsWithinRange()
{
	TArray<AActor*> TempArray;

	if (TargetingActors.Num() > 0)
	{
		for (AActor* Actor : TargetingActors)
		{
			APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			if (Actor
				&& FVector::Dist(Actor->GetActorLocation(), GetActorLocation()) <= MaxTargetingDistance
				&& PC
				&& PC->LineOfSightTo(Actor)) //make sure we can actually see the bastard, so that we don't get a target behind us. CanSee can potentially be used too, but then we need a Pawn	
			{
				TempArray.Add(Actor);
			}
		}
	}

	return TempArray;
}

void ACPPPlayerCharacter::GetNextTarget()
{
	if (bIsTargeting)
	{
		TArray<AActor*> TargetingActorsWithinRange = GetAllTargetingActorsWithinRange();
		OldTargetingActor = TargetingActor;

		if (TargetingActorsWithinRange.Num() == 0)
		{
			ToggleTargeting();
			return;
		}

		//make sure our targeting index is not bigger than the current array, since that can happen. 
		if (TargetingIndex >= TargetingActorsWithinRange.Num())
			TargetingIndex = TargetingActorsWithinRange.Num() - 1;

		if (TargetingActorsWithinRange.Num() == 1)
		{
			TargetingActor = TargetingActorsWithinRange[0];
		}
		else if (TargetingActorsWithinRange.Num() > 0)
		{
			int32 Failsafe = 0;

			while (Failsafe < TargetingActorsWithinRange.Num())
			{
				TargetingIndex++;
				Failsafe++;

				if (TargetingIndex >= TargetingActorsWithinRange.Num())
				{
					TargetingIndex = 0;
				}

				if (TargetingActorsWithinRange[TargetingIndex] != TargetingActor)
				{
					TargetingActor = TargetingActorsWithinRange[TargetingIndex];
					break; //rather than a bool, we should break the loop entirely here. 
				}
			}
		}
		else
		{
			TargetingIndex = -1;
		}

		//Finally set the required UI elements
		ACPPEnemyCharacter* OldEnemy = Cast<ACPPEnemyCharacter>(OldTargetingActor);
		if (OldEnemy)
			OldEnemy->EnemyHealthUI->SetVisibility(ESlateVisibility::Hidden);

		ACPPEnemyCharacter* NewEnemy = Cast<ACPPEnemyCharacter>(TargetingActor);
		if(NewEnemy)
			NewEnemy->EnemyHealthUI->SetVisibility(ESlateVisibility::Visible);

	}
}

void ACPPPlayerCharacter::GetPreviousTarget()
{
	if (bIsTargeting)
	{
		TArray<AActor*> TargetingActorsWithinRange = GetAllTargetingActorsWithinRange();
		OldTargetingActor = TargetingActor;

		if (TargetingActorsWithinRange.Num() == 0)
		{
			ToggleTargeting();
			return;
		}

		//make sure our targeting index is not bigger than the current array, since that can happen. 
		if (TargetingIndex >= TargetingActorsWithinRange.Num())
			TargetingIndex = TargetingActorsWithinRange.Num() - 1;

		if (TargetingActorsWithinRange.Num() == 1)
		{
			TargetingActor = TargetingActorsWithinRange[0];
		}
		else if (TargetingActorsWithinRange.Num() > 0)
		{
			int32 Failsafe = 0;

			while (Failsafe < TargetingActorsWithinRange.Num())
			{
				TargetingIndex--;
				Failsafe++;

				if (TargetingIndex < 0)
				{
					TargetingIndex = TargetingActorsWithinRange.Num() - 1;
				}

				if (TargetingActorsWithinRange[TargetingIndex] != TargetingActor)
				{
					TargetingActor = TargetingActorsWithinRange[TargetingIndex];
					break; //rather than a bool, we should break the loop entirely here. 
				}
			}
		}
		else
		{
			TargetingIndex = -1;
		}

		//Finally set the required UI elements
		ACPPEnemyCharacter* OldEnemy = Cast<ACPPEnemyCharacter>(OldTargetingActor);
		if (OldEnemy)
			OldEnemy->EnemyHealthUI->SetVisibility(ESlateVisibility::Hidden);

		ACPPEnemyCharacter* NewEnemy = Cast<ACPPEnemyCharacter>(TargetingActor);
		if (NewEnemy)
			NewEnemy->EnemyHealthUI->SetVisibility(ESlateVisibility::Visible);
	}
}

void ACPPPlayerCharacter::ToggleTargeting()
{
	if (bIsTargeting)
	{
		bIsTargeting = false;

		//First get rid of the old targeting actors and their UI
		OldTargetingActor = nullptr;
		
		ACPPEnemyCharacter* Enemy = Cast<ACPPEnemyCharacter>(TargetingActor);
		if (Enemy)
			Enemy->EnemyHealthUI->SetVisibility(ESlateVisibility::Hidden);

		//Then switch back to our main camera
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PC && TargetCamera) //even though we don't use it, we still do a check for target camera, since if we don't have a camera, we don't need to set VT to start with
		{
			FViewTargetTransitionParams VTParams;
			VTParams.BlendTime = 0.2f;
			VTParams.BlendFunction = EViewTargetBlendFunction::VTBlend_Linear;

			PC->SetViewTarget(this, VTParams);
		}
	}
	else
	{
		TArray<AActor*> TargetingActorsWithinRange = GetAllTargetingActorsWithinRange();
		float CurrentShortestDistance = 0.f;

		if (TargetingActorsWithinRange.Num() == 0) return; //break the process if we don't have anything to target.

		//Get the closest target to the player, and make that the new TargetIndex.
		for (int32 i = 0; i < TargetingActorsWithinRange.Num(); i++)
		{
			if (FVector::Dist(TargetingActorsWithinRange[i]->GetActorLocation(), GetActorLocation()) <= CurrentShortestDistance || CurrentShortestDistance == 0.f)
			{
				CurrentShortestDistance = FVector::Dist(TargetingActorsWithinRange[i]->GetActorLocation(), GetActorLocation());
				TargetingActor = TargetingActorsWithinRange[i];
				TargetingIndex = i;

				ACPPEnemyCharacter* Enemy = Cast<ACPPEnemyCharacter>(TargetingActor);
				if (Enemy)
					Enemy->EnemyHealthUI->SetVisibility(ESlateVisibility::Visible);
			}
		}

		//Switch cameras
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PC && TargetCamera)
		{
			FViewTargetTransitionParams VTParams;
			VTParams.BlendTime = 0.2f;
			VTParams.BlendFunction = EViewTargetBlendFunction::VTBlend_Linear;

			PC->SetViewTarget(TargetCamera, VTParams);
		}

		bIsTargeting = true; //make sure to set this to true at the end, so that we are sure all the basic data is set before we can switch targets
	}
}

void ACPPPlayerCharacter::RemoveTarget(AActor* Target)
{
	int32 TargetIndex = TargetingActors.Find(Target);

	UE_LOG(AnyLog, Warning, TEXT("Trying to remove target %s, array index is %d"), *(Target->GetName()), TargetIndex);

	if (TargetIndex != -1)
	{
		//simply remove from total array, since we need to update that bastard
		TargetingActors.Remove(Target);
	}

	//however, if we are targeting, we need to update the current targets.
	if (bIsTargeting)
		GetNextTarget();
}

void ACPPPlayerCharacter::MoveForward(float Value)
{
	if (bCanMove)
	{
		if (Controller != nullptr)
		{
			if (!bIsTargeting)
			{
				if (Value != 0.0f)
				{
					// find out which way is forward
					const FRotator Rotation = Controller->GetControlRotation();
					const FRotator YawRotation(0, Rotation.Yaw, 0);

					// get forward vector
					const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
					AddMovementInput(Direction, Value);
				}
			}
			else if (TargetingActor)
			{
				//First see if we still need targeting. 
				if (FVector::Dist(TargetingActor->GetActorLocation(), GetActorLocation()) > MaxTargetingDistance * 1.35)
				{
					ToggleTargeting();
					return;
				}

				////Then lets declare some necessary variables, and initialize where possible. 
				//FVector TargetLocation = TargetingActor->GetActorLocation();
				//TargetLocation.Z = GetActorLocation().Z; //equal out the Z to the player, so we don't get strange rotations when we set actor rot later. 
				//FVector TargetDimension, RotateValue;
				//FRotator TargetRotation;
				//float TargetAngle;
				//float DeltaSeconds = GetWorld()->GetDeltaSeconds(); //set this up in advance to reduce redudant calls later.
				//float TargetDistance = FVector::Dist2D(TargetingActor->GetActorLocation(), GetActorLocation()); //initialize target distance.
				////we need to initialize the target distance since the enemy could move and we don't want to have the player get dragged along.

				////Setup dimensions
				//TargetDistanceIncrease = TargetDistance + (Value * -1.f) * MaxStrafeSpeed * DeltaSeconds; //Since we use the target distance increase elsewhere, completely calculate it here
				//TargetDimension = FVector(TargetDistanceIncrease, 0.f, 0.f); //we want the x axis here as that corresponds with the axis that is getting offsetted

				////First we get the lookat rotation, so we can use the yaw for further calculations
				//TargetRotation = FRotationMatrix::MakeFromX(TargetLocation - GetActorLocation()).Rotator(); //this gets the heading towards the 

				////Calculate the necessary increase in angle based on input.
				//TargetAngle = (TargetRotation.Yaw - 180.f) + (TargetAngleIncrease * DeltaSeconds); //we need to take off 180, since we need to swap perspectives (not player -> target, but the other way around)
				//RotateValue = TargetDimension.RotateAngleAxis(TargetAngle, TargetingVector); //This rotates TargetDimension around the TargetingVector using TargetAngle degrees
				//TargetLocation += RotateValue;

				////Setup rotation again
				//FRotator AnimTargetRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaSeconds, 5.f); //create animated rotation
				//FQuat QuatRot = FQuat(AnimTargetRotation); //and turn it into a quat so we can apply it later (we could also just use SetActorRotation again).

				float DeltaSeconds = GetWorld()->GetDeltaSeconds();
				TargetDistanceIncrease = (Value * -1.f) * MaxStrafeSpeed * DeltaSeconds;
				float TargetDistance = FVector::Dist2D(TargetingActor->GetActorLocation(), GetActorLocation()) + TargetDistanceIncrease; //we have no choice but to recalculate this, since we need this for calculating the angle increase
				TargetAngleIncrease = (MoveRightValue * -1.f) * (360.f / ((TargetDistance * 2.f * PI) / MaxStrafeSpeed)) * DeltaSeconds;
				FTransform TargetTransform = UCPPFunctionLibrary::RotateActorAroundTarget(this, TargetingActor, TargetDistanceIncrease, TargetAngleIncrease);
				FRotator AnimTargetRotation = FMath::RInterpTo(GetActorRotation(), TargetTransform.GetRotation().Rotator(), DeltaSeconds, 5.f); //create animated rotation

				SetActorLocationAndRotation(TargetTransform.GetTranslation(), FQuat(AnimTargetRotation), false, 0, ETeleportType::None);
				StrafeVelocity = FVector((MoveRightValue * -1.f) * MaxStrafeSpeed, (Value * -1.f) * MaxStrafeSpeed, 0.f);
			}
		}
	}
}

void ACPPPlayerCharacter::MoveRight(float Value)
{
	if (bCanMove)
	{
		if (Controller != nullptr)
		{
			if (!bIsTargeting)
			{
				if (Value != 0.0f)
				{
					// find out which way is right
					const FRotator Rotation = Controller->GetControlRotation();
					const FRotator YawRotation(0, Rotation.Yaw, 0);

					// get right vector 
					const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
					// add movement in that direction
					AddMovementInput(Direction, Value);
				}
			}
			else if (TargetingActor)
			{
				MoveRightValue = Value;
			}
		}
	}
}

//DAMAGE

void ACPPPlayerCharacter::TakePointDamage(class AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* HitComponent, FName BoneName, FVector HitFromDirection, const class UDamageType* DamageType, class AActor* DamageCauser)
{
	UE_LOG(AnyLog, Warning, TEXT("Damaged! %f"), Damage);

	if(!bIsDead
		&& DamageAnimationState == EDamageWeight::PDA_None) //so we cannot be damaged while under damage animations
	{
		CurrentHealth -= Damage;

		OnDamage(MaxHealth, FMath::Max(CurrentHealth, 0.f));

		if (CurrentHealth <= 0.f)
		{
			OnDeath();
			return; //make sure to just stop when dead.
		}	

		const UCPPDamageBase* DamageTypeBase = Cast<UCPPDamageBase>(DamageType);
		if (DamageTypeBase
			&& GetWorld())
		{
			FLatentActionInfo ActionInfo;
			ActionInfo.CallbackTarget = this;
			ActionInfo.ExecutionFunction = "OnFinishDamageCooldown";
			ActionInfo.UUID = 1;
			ActionInfo.Linkage = 0;

			FVector LaunchVelocity;
			APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			if(!PC) return;

			//Set rotation to face the damage. 
			FVector ImpactModified = HitFromDirection;
			ImpactModified.Z = 0.f; //make sure we get only XY rotations.
			ImpactModified *= -1.f; //make sure to reverse the vector too.
			FRotator NewRot = ImpactModified.Rotation();
			SetActorRotation(NewRot);

			DamageAnimationState = DamageTypeBase->DamageWeight;
			switch (DamageAnimationState)
			{
			case EDamageWeight::PDA_None:
				break;

			case EDamageWeight::PDA_Light:
				LaunchVelocity = HitFromDirection * 400.f;
				LaunchCharacter(LaunchVelocity, true, false);
				UKismetSystemLibrary::Delay(GetWorld(), 0.5f, ActionInfo);
				break;

			case EDamageWeight::PDA_Heavy:
				LaunchVelocity = HitFromDirection * 750.f;
				LaunchVelocity.Z = 500.f;
				LaunchCharacter(LaunchVelocity, true, true);
				RequestedDamageRecoveryTime = 2.0f;
				break;
			}

			//Disable the input so we cannot do anything during damage.
			DisableInput(PC);
		}
	}
}

void ACPPPlayerCharacter::Landed(const FHitResult& Hit)
{
	UE_LOG(AnyLog, Warning, TEXT("Landed!"));

	if (DamageAnimationState != EDamageWeight::PDA_None
		&& RequestedDamageRecoveryTime > 0.f)
	{
		FLatentActionInfo ActionInfo;
		ActionInfo.CallbackTarget = this;
		ActionInfo.ExecutionFunction = "OnFinishDamageCooldown";
		ActionInfo.UUID = 1;
		ActionInfo.Linkage = 0;

		UKismetSystemLibrary::Delay(GetWorld(), RequestedDamageRecoveryTime, ActionInfo);
	}

	Super::Landed(Hit);
}

void ACPPPlayerCharacter::OnFinishDamageCooldown()
{
	UE_LOG(AnyLog, Warning, TEXT("Cooldown has ended!"));

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	DamageAnimationState = EDamageWeight::PDA_None;

	if (PC)
		EnableInput(PC);
}

void ACPPPlayerCharacter::OnDeath()
{
	Super::OnDeath();

	bIsDead = true;
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if(PC) 
		DisableInput(PC);
}
// Fill out your copyright notice in the Description page of Project Settings.

#include "Labyrinth.h"
#include "LabyrinthCharacter.h"
#include "LabyrinthCharMoveComp.h"


// Sets default values
ALabyrinthCharacter::ALabyrinthCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<ULabyrinthCharMoveComp>(ACharacter::CharacterMovementComponentName))
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Get the Character Movement Component
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	MoveComp->GravityScale = 1.5f;
	MoveComp->JumpZVelocity = 620.0f;
	MoveComp->bCanWalkOffLedgesWhenCrouching = true;
	MoveComp->MaxWalkSpeedCrouched = 200.0f;
	// Enable Crouching
	MoveComp->GetNavAgentPropertiesRef().bCanCrouch = true;

	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_PROJECTILE, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	BaseTurnRate = 45.0f;
	BaseLookUpRate = 45.0f;

	CameraBoomComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("BoomComp"));
	CameraBoomComp->SocketOffset = FVector(0, 35, 0);
	CameraBoomComp->TargetOffset = FVector(0, 0, 55);
	CameraBoomComp->bUsePawnControlRotation = true;
	CameraBoomComp->AttachParent = GetRootComponent();

	CameraComponent1P = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	CameraComponent1P->AttachParent = CameraBoomComp;
	/*CameraComponent1P->RelativeLocation = FVector(0, 0, 64.0f);
	CameraComponent1P->bUsePawnControlRotation = true;*/

	/*Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->AttachParent = GetFirstPersonCameraComponent();
	Mesh1P->RelativeLocation = FVector(0, 0, -150.0f);
	Mesh1P->bCastDynamicShadow = false;*/

	MaxUseDistance = 100.0f;
	DropItemDistance = 50.0f;
	bHasNewFocus = true;
	TargetingSpeedModifier = 0.5f;
	SprintingSpeedModifier = 2.5f;

	Health = 100;

	IncrementHungerAmount = 1.0f;
	IncrementHungerInterval = 5.0f;
	CriticalHungerThreshold = 90;

	MaxHunger = 100;
	Hunger = 0;

	WeaponAttachPoint = TEXT("WeaponSocket");
	PelvisAttachPoint = TEXT("PelvisSocket");
	SpineAttachPoint = TEXT("SpineSocket");
}

void ALabyrinthCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (Role == ROLE_Authority)
	{
		FTimerHandle Timer;
		// Add a timer for the hunger to start kicking in
		GetWorld()->GetTimerManager().SetTimer(Timer, this, &ALabyrinthCharacter::IncrementHunger, IncrementHungerInterval, true);

		SpawnDefaultInventory();
	}
}

void ALabyrinthCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Value is already updated locally, ski in replication step
	DOREPLIFETIME_CONDITION(ALabyrinthCharacter, bWantsToRun, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ALabyrinthCharacter, bIsTargeting, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ALabyrinthCharacter, bIsJumping, COND_SkipOwner);

	DOREPLIFETIME(ALabyrinthCharacter, Health);
	DOREPLIFETIME(ALabyrinthCharacter, Hunger);
	DOREPLIFETIME(ALabyrinthCharacter, LastTakeHitInfo);
	DOREPLIFETIME(ALabyrinthCharacter, CurrentWeapon);
	DOREPLIFETIME(ALabyrinthCharacter, FoodInventory);
}

// Called when the game starts or when spawned
void ALabyrinthCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALabyrinthCharacter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (bWantsToRun && !IsSprinting())
	{
		SetSprinting(true);
	}

	if (Controller && Controller->IsLocalController())
	{
		AUsableActor* usable = GetUsableInView();

		//End Focus
		if (FocusedUsableActor != usable)
		{
			if (FocusedUsableActor)
			{
				FocusedUsableActor->EndFocusItem();
			}
			bHasNewFocus = true;
		}

		FocusedUsableActor = usable;

		//Start Focus
		if (usable)
		{
			if (bHasNewFocus)
			{
				usable->StartFocusItem();
				bHasNewFocus = false;
			}
		}
	}
}

void ALabyrinthCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	DestroyInventory();
}


void ALabyrinthCharacter::Use_Implementation()
{
	AUsableActor* usable = GetUsableInView();
	if (usable)
	{
		ALWeaponPickup* WP = Cast<ALWeaponPickup>(usable);
		if (WP)
		{
			WP->OnUsed(this);
		}
		else
		{
			usable->OnUsed(this);
		}
		
	}
}

bool ALabyrinthCharacter::Use_Validate()
{
	return true;
}

AUsableActor* ALabyrinthCharacter::GetUsableInView()
{
	FVector CamLoc;
	FRotator CamRot;

	if (Controller == NULL)
		return NULL;

	Controller->GetPlayerViewPoint(CamLoc, CamRot);
	const FVector start_trace = CamLoc;
	const FVector direction = CamRot.Vector();
	const FVector end_trace = start_trace + (direction * MaxUseDistance);

	FCollisionQueryParams TraceParams(FName(TEXT("")), true, this);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.bTraceComplex = true;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingle(Hit, start_trace, end_trace, COLLISION_PROJECTILE, TraceParams);

	return Cast<AUsableActor>(Hit.GetActor());
}

// Called to bind functionality to input
void ALabyrinthCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	check(InputComponent);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ALabyrinthCharacter::OnStartJump);
	InputComponent->BindAction("Jump", IE_Released, this, &ALabyrinthCharacter::OnStopJump);

	InputComponent->BindAction("SprintHold", IE_Pressed, this, &ALabyrinthCharacter::OnStartSprinting);
	InputComponent->BindAction("SprintHold", IE_Pressed, this, &ALabyrinthCharacter::OnStopSprinting);

	InputComponent->BindAction("CrouchToggle", IE_Released, this, &ALabyrinthCharacter::OnCrouchToggle);

	InputComponent->BindAction("DropWeapon", IE_Released, this, &ALabyrinthCharacter::DropWeapon);

	InputComponent->BindAction("Targeting", IE_Pressed, this, &ALabyrinthCharacter::OnStartTargeting);
	InputComponent->BindAction("Targeting", IE_Released, this, &ALabyrinthCharacter::OnEndTargeting);

	InputComponent->BindAction("NextWeapon", IE_Pressed, this, &ALabyrinthCharacter::OnNextWeapon);
	InputComponent->BindAction("PrevWeapon", IE_Pressed, this, &ALabyrinthCharacter::OnPrevWeapon);

	InputComponent->BindAction("EquipPrimaryWeapon", IE_Pressed, this, &ALabyrinthCharacter::OnEquipPrimaryWeapon);
	InputComponent->BindAction("EquipSecondaryWeapon", IE_Pressed, this, &ALabyrinthCharacter::OnEquipSecondaryWeapon);

	/*if (EnableTouchscreenMovement(InputComponent) == false)
	{
		InputComponent->BindAction("Fire", IE_Pressed, this, &ALabyrinthCharacter::OnStartFire);
		InputComponent->BindAction("Fire", IE_Released, this, &ALabyrinthCharacter::OnStopFire);
	}*/

	InputComponent->BindAction("Fire", IE_Pressed, this, &ALabyrinthCharacter::OnStartFire);
	InputComponent->BindAction("Fire", IE_Released, this, &ALabyrinthCharacter::OnStopFire);

	InputComponent->BindAxis("MoveForward", this, &ALabyrinthCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ALabyrinthCharacter::MoveRight);

	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &ALabyrinthCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &ALabyrinthCharacter::LookUpAtRate);

	InputComponent->BindAction("Use", IE_Pressed, this, &ALabyrinthCharacter::Use);
}

void ALabyrinthCharacter::OnFire()
{

}

void ALabyrinthCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void ALabyrinthCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = false;
}

void ALabyrinthCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if ((TouchItem.bIsPressed == true) && ( TouchItem.FingerIndex==FingerIndex))
	{
		if (TouchItem.bIsPressed)
		{
			if (GetWorld() != nullptr)
			{
				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
				if (ViewportClient != nullptr)
				{
					FVector MoveDelta = Location - TouchItem.Location;
					FVector2D ScreenSize;
					ViewportClient->GetViewportSize(ScreenSize);
					FVector2D ScaledDelta = FVector2D( MoveDelta.X, MoveDelta.Y) / ScreenSize;									
					if (ScaledDelta.X != 0.0f)
					{
						TouchItem.bMoved = true;
						float Value = ScaledDelta.X * BaseTurnRate;
						AddControllerYawInput(Value);
					}
					if (ScaledDelta.Y != 0.0f)
					{
						TouchItem.bMoved = true;
						float Value = ScaledDelta.Y* BaseTurnRate;
						AddControllerPitchInput(Value);
					}
					TouchItem.Location = Location;
				}
				TouchItem.Location = Location;
			}
		}
	}
}

void ALabyrinthCharacter::MoveForward(float Val)
{
	if (Val != 0.0f)
	{
		// Limit Rotation when walking or falling
		const bool bLimitRotation = (GetCharacterMovement()->IsMovingOnGround() || (GetCharacterMovement()->IsFalling()));
		const FRotator Rotation = bLimitRotation ? GetActorRotation() : Controller->GetControlRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);

		AddMovementInput(Direction, Val);
	}
}

void ALabyrinthCharacter::MoveRight(float Val)
{
	if (Val != 0.0f)
	{
		const FRotator Rotation = GetActorRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
		AddMovementInput(Direction, Val);
	}
}

void ALabyrinthCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ALabyrinthCharacter::LookUpAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool ALabyrinthCharacter::EnableTouchscreenMovement(class UInputComponent* InputComponent)
{
	bool bResult = false;
	if (FPlatformMisc::GetUseVirtualJoysticks() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		bResult = true;
		InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ALabyrinthCharacter::BeginTouch);
		InputComponent->BindTouch(EInputEvent::IE_Released, this, &ALabyrinthCharacter::EndTouch);
		InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ALabyrinthCharacter::TouchUpdate);
	}

	return bResult;
}

void ALabyrinthCharacter::OnStartTargeting()
{
	SetTargeting(true);
}

void ALabyrinthCharacter::OnEndTargeting()
{
	SetTargeting(false);
}

void ALabyrinthCharacter::SetTargeting(bool NewTargeting)
{
	bIsTargeting = NewTargeting;

	if (Role < ROLE_Authority)
	{
		ServerSetTargeting(NewTargeting);
	}
}

void ALabyrinthCharacter::ServerSetTargeting_Implementation(bool NewTargeting)
{
	SetTargeting(NewTargeting);
}

bool ALabyrinthCharacter::ServerSetTargeting_Validate(bool NewTargeting)
{
	return true;
}

bool ALabyrinthCharacter::IsTargeting() const
{
	return bIsTargeting;
}

float ALabyrinthCharacter::GetTargetingSpeedModifier() const
{
	return TargetingSpeedModifier;
}

void ALabyrinthCharacter::OnStartJump()
{
	bPressedJump = true;
	SetIsJumping(true);
}

void ALabyrinthCharacter::OnStopJump()
{
	bPressedJump = false;
}

bool ALabyrinthCharacter::IsInitiatedJump() const
{
	return bIsJumping;
}

void ALabyrinthCharacter::SetIsJumping(bool NewJumping)
{
	if (bIsCrouched && NewJumping)
	{
		UnCrouch();
	}
	else
	{
		bIsJumping = NewJumping;
	}

	if (Role < ROLE_Authority)
	{
		ServerSetIsJumping(NewJumping);
	}
}

void ALabyrinthCharacter::OnLanded(const FHitResult& Hit)
{
	Super::OnLanded(Hit);

	SetIsJumping(false);
}

void ALabyrinthCharacter::ServerSetIsJumping_Implementation(bool NewJumping)
{
	SetIsJumping(NewJumping);
}

bool ALabyrinthCharacter::ServerSetIsJumping_Validate(bool NewJumping)
{
	return true;
}

void ALabyrinthCharacter::SetSprinting(bool NewSprinting)
{
	bWantsToRun = NewSprinting;

	if (bIsCrouched)
		UnCrouch();

	if (Role < ROLE_Authority)
	{
		ServerSetSprinting(NewSprinting);
	}
}

void ALabyrinthCharacter::OnStartSprinting()
{
	SetSprinting(true);
}

void ALabyrinthCharacter::OnStopSprinting()
{
	SetSprinting(false);
}

void ALabyrinthCharacter::ServerSetSprinting_Implementation(bool NewSprinting)
{
	SetSprinting(NewSprinting);
}

bool ALabyrinthCharacter::ServerSetSprinting_Validate(bool NewSprinting)
{
	return true;
}

bool ALabyrinthCharacter::IsSprinting() const
{
	if (!GetCharacterMovement())
		return false;

	return bWantsToRun && !IsTargeting() && !GetVelocity().IsZero() && (GetVelocity().GetSafeNormal2D() | GetActorRotation().Vector()) > 0.8;
}

float ALabyrinthCharacter::GetSprintingSpeedModifier() const
{
	return SprintingSpeedModifier;
}

void ALabyrinthCharacter::OnCrouchToggle()
{
	if (CanCrouch())
	{
		Crouch();
	}
	else
	{
		UnCrouch();
	}
}

FRotator ALabyrinthCharacter::GetAimOffsets() const
{
	const FVector AimDirWS = GetBaseAimRotation().Vector();
	const FVector AimDirLS = ActorToWorld().InverseTransformVectorNoScale(AimDirWS);
	const FRotator AimRotLS = AimDirLS.Rotation();

	return AimRotLS;
}

float ALabyrinthCharacter::GetHealth() const
{
	return Health;
}

float ALabyrinthCharacter::GetHunger() const
{
	return Hunger;
}

float ALabyrinthCharacter::GetMaxHealth() const
{
	// Retrieve the default value of the health assigned to this character
	return GetClass()->GetDefaultObject<ALabyrinthCharacter>()->Health;
}

float ALabyrinthCharacter::GetMaxHunger() const
{
	return MaxHunger;
}

void ALabyrinthCharacter::ConsumeFood(float AmountRestored)
{
	// Reduce Hunger, while making sure we are within bounds
	Hunger = FMath::Clamp(Hunger - AmountRestored, 0.0f, GetMaxHunger());

	// Restore Hit Points
	Health = FMath::Clamp(Health + AmountRestored, 0.0f, GetMaxHealth());

	APlayerController* PC = Cast<APlayerController>(Controller);
	if (PC)
	{
		// HUD Stuff comes in here, as and when I implement it
	}
}

bool ALabyrinthCharacter::IsAlive() const
{
	return Health > 0;
}

void ALabyrinthCharacter::IncrementHunger()
{
	Hunger = FMath::Clamp(Hunger + IncrementHungerAmount, 0.0f, GetMaxHunger());

	if (Hunger > CriticalHungerThreshold)
	{
		// Apply Damage to self
		TakeDamage(10.0f, FDamageEvent(), GetController(), this);
	}
}

float ALabyrinthCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	if (Health < 0.0f)
	{
		return 0.f;
	}

	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f)
	{
		Health -= ActualDamage;
		if (Health <= 0.f)
		{
			Die(ActualDamage, DamageEvent, EventInstigator, DamageCauser);
		}
		else
		{
			/* Shorthand for if x != NULL pick1 else pick2 */
			APawn* Pawn = EventInstigator ? EventInstigator->GetPawn() : nullptr;
			PlayHit(ActualDamage, DamageEvent, Pawn, DamageCauser, false);
		}
	}

	GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Red, this->GetName() + "health: " + FString::SanitizeFloat(Health));
	return ActualDamage;
}

bool ALabyrinthCharacter::CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser) const
{
	// Check if the character is already dying, destroyed or if we have authority
	if (bIsDying ||
		IsPendingKill() ||
		Role != ROLE_Authority ||
		GetWorld()->GetAuthGameMode() == NULL ||
		GetWorld()->GetAuthGameMode()->GetMatchState() == MatchState::LeavingMap)
	{
		return false;
	}

	return true;
}

bool ALabyrinthCharacter::Die(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser)
{
	if (!CanDie(KillingDamage, DamageEvent, Killer, DamageCauser))
	{
		return false;
	}

	Health = FMath::Min(0.0f, Health);

	// Fallback to default DamageType if None is specified
	UDamageType const* const DamageType = DamageEvent.DamageTypeClass ? DamageEvent.DamageTypeClass->GetDefaultObject<UDamageType>() : GetDefault<UDamageType>();
	Killer = GetDamageInstigator(Killer, *DamageType);

	OnDeath(KillingDamage, DamageEvent, Killer ? Killer->GetPawn() : NULL, DamageCauser);
	return true;
}

void ALabyrinthCharacter::OnDeath(float KillingDamage, FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser)
{
	if (bIsDying)
	{
		return;
	}

	bReplicateMovement = false;
	bTearOff = true;
	bIsDying = true;

	PlayHit(KillingDamage, DamageEvent, PawnInstigator, DamageCauser, true);

	DestroyInventory();

	DetachFromControllerPendingDestroy();
	StopAllAnimMontages();

	// Disable collision on capsule
	UCapsuleComponent* CapComp = GetCapsuleComponent();
	CapComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	USkeletalMeshComponent* SkelMesh = GetMesh();
	if (SkelMesh)
	{
		SkelMesh->SetCollisionProfileName(TEXT("Ragdoll"));
	}

	SetActorEnableCollision(true);

	SetRagdollPhysics();
}

void ALabyrinthCharacter::SetRagdollPhysics()
{
	bool bInRagdoll = false;
	USkeletalMeshComponent* SkelMesh = GetMesh();

	if (IsPendingKill())
	{
		bInRagdoll = false;
	}
	else if (!SkelMesh || !SkelMesh->GetPhysicsAsset())
	{
		bInRagdoll = false;
	}
	else
	{
		SkelMesh->SetAllBodiesSimulatePhysics(true);
		SkelMesh->SetSimulatePhysics(true);
		SkelMesh->WakeAllRigidBodies();
		SkelMesh->bBlendPhysics = true;

		bInRagdoll = true;
	}

	UCharacterMovementComponent* CharMoveComp = Cast<UCharacterMovementComponent>(GetMovementComponent());
	if (CharMoveComp)
	{
		CharMoveComp->StopMovementImmediately();
		CharMoveComp->DisableMovement();
		CharMoveComp->SetComponentTickEnabled(false);
	}

	if (!bInRagdoll)
	{
		// Hide the pawn
		TurnOff();
		SetActorHiddenInGame(true);
		SetLifeSpan(1.0f);
	}
	else
	{
		SetLifeSpan(10.0f);
	}
}

void ALabyrinthCharacter::FellOutOfWorld(const class UDamageType& DmgType)
{
	Die(Health, FDamageEvent(DmgType.GetClass()), NULL, NULL);
}

void ALabyrinthCharacter::PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser, bool bKilled)
{
	if (Role == ROLE_Authority)
	{
		ReplicateHit(DamageTaken, DamageEvent, PawnInstigator, DamageCauser, bKilled);
	}

	// Apply Damage momentum specific to the damage type
	if (DamageTaken > 0.f)
	{
		ApplyDamageMomentum(DamageTaken, DamageEvent, PawnInstigator, DamageCauser);
	}
}

void ALabyrinthCharacter::ReplicateHit(float DamageTaken, struct FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser, bool bKilled)
{
	const float TimeoutTime = GetWorld()->GetTimeSeconds() + 0.5f;

	FDamageEvent const& LastDamageEvent = LastTakeHitInfo.GetDamageEvent();
	if (PawnInstigator == LastTakeHitInfo.PawnInstigator.Get() && LastDamageEvent.DamageTypeClass == LastTakeHitInfo.DamageTypeClass)
	{
		// Same frame Damage
		if (bKilled && LastTakeHitInfo.bIsKilled)
		{
			// Redundant death take hit, ignore
			return;
		}

		DamageTaken += LastTakeHitInfo.ActualDamage;
	}

	LastTakeHitInfo.ActualDamage = DamageTaken;
	LastTakeHitInfo.PawnInstigator = Cast<ALabyrinthCharacter>(PawnInstigator);
	LastTakeHitInfo.DamageCauser = DamageCauser;
	LastTakeHitInfo.SetDamageEvent(DamageEvent);
	LastTakeHitInfo.bIsKilled = bKilled;
	LastTakeHitInfo.EnsureReplication();
}

void ALabyrinthCharacter::OnRep_LastTakeHitInfo()
{
	if (LastTakeHitInfo.bIsKilled)
	{
		OnDeath(LastTakeHitInfo.ActualDamage, LastTakeHitInfo.GetDamageEvent(), LastTakeHitInfo.PawnInstigator.Get(), LastTakeHitInfo.DamageCauser.Get());
	}
	else
	{
		PlayHit(LastTakeHitInfo.ActualDamage, LastTakeHitInfo.GetDamageEvent(), LastTakeHitInfo.PawnInstigator.Get(), LastTakeHitInfo.DamageCauser.Get(), LastTakeHitInfo.bIsKilled);
	}
}

bool ALabyrinthCharacter::CanFire() const
{
	// We can add our own checks here, eg. powers are recharging, etc.
	return IsAlive();
}

bool ALabyrinthCharacter::IsFiring() const
{
	//return CurrentWeapon && CurrentWeapon->GetCurrentState() == EWeaponState::Firing;
	return false;
}

FName ALabyrinthCharacter::GetInventoryAttachPoint(EInventorySlot Slot) const
{
	// Return the socke name for the specified storage slot
	switch (Slot)
	{
	case EInventorySlot::Hands :
		return WeaponAttachPoint;
	case EInventorySlot::Primary :
		return SpineAttachPoint;
	case EInventorySlot::Secondary :
		return PelvisAttachPoint;
	default :
		// Not Implemented
		return "";
	}
}

void ALabyrinthCharacter::SpawnDefaultInventory()
{
	if (Role < ROLE_Authority)
	{
		return;
	}

	for (int32 i = 0; i < DefaultInventoryClasses.Num(); i++)
	{
		if (DefaultInventoryClasses[i])
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.bNoCollisionFail = true;
			ALWeapon* NewWeapon = GetWorld()->SpawnActor<ALWeapon>(DefaultInventoryClasses[i], SpawnInfo);

			AddWeapon(NewWeapon);
		}
	}
}

void ALabyrinthCharacter::DestroyInventory()
{
	if (Role < ROLE_Authority)
	{
		return;
	}

	//for (int32 i = Inventory.Num() - 1; i >= 0; i++)
	//{
	//	// Change this to ALWeapon
	//	ALWeapon* Weapon = Inventory[i];
	//	if (Weapon)
	//	{
	//		RemoveWeapon(Weapon);
	//		Weapon->Destroy();
	//	}
	//}
}

void ALabyrinthCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();

	SetCurrentWeapon(CurrentWeapon);
}

void ALabyrinthCharacter::OnStartFire()
{
	if (IsSprinting())
	{
		SetSprinting(false);
	}
	StartWeaponFire();
}

void ALabyrinthCharacter::OnStopFire()
{
	StopWeaponFire();
}

void ALabyrinthCharacter::StartWeaponFire()
{
	if (!bWantsToFire)
	{
		bWantsToFire = true;
		if (CurrentWeapon)
		{
			CurrentWeapon->StartFire();
		}
	}
}

void ALabyrinthCharacter::StopWeaponFire()
{
	if (bWantsToFire)
	{
		bWantsToFire = false;
		if (CurrentWeapon)
		{
			CurrentWeapon->StopFire();
		}
	}
}

void ALabyrinthCharacter::OnNextWeapon()
{
	if (Inventory.Num() >= 2)
	{
		const int32 CurrentWeaponIndex = Inventory.IndexOfByKey(CurrentWeapon);
		ALWeapon* NextWeapon = Inventory[(CurrentWeaponIndex + 1) % Inventory.Num()];
		EquipWeapon(NextWeapon);
	}
}

void ALabyrinthCharacter::OnPrevWeapon()
{
	if (Inventory.Num() >= 2)
	{
		const int32 CurrentWeaponIndex = Inventory.IndexOfByKey(CurrentWeapon);
		ALWeapon* PrevWeapon = Inventory[(CurrentWeaponIndex - 1 + Inventory.Num()) % Inventory.Num()];
		EquipWeapon(PrevWeapon);
	}
}

void ALabyrinthCharacter::DropWeapon()
{
	if (Role < ROLE_Authority)
	{
		ServerDropWeapon();
		return;
	}

	if (CurrentWeapon)
	{
		FVector CamLoc;
		FRotator CamRot;

		if (Controller == nullptr)
			return;

		// Find a location to drop the weapon, slightly in front of the player
		Controller->GetPlayerViewPoint(CamLoc, CamRot);
		const FVector Direction = CamRot.Vector();
		const FVector SpawnLocation = GetActorLocation() + (Direction * DropItemDistance);

		FActorSpawnParameters SpawnInfo;
		SpawnInfo.bNoCollisionFail = true;
		ALWeaponPickup* NewWeaponPickup = GetWorld()->SpawnActor<ALWeaponPickup>(CurrentWeapon->WeaponPickupClass, SpawnLocation, FRotator::ZeroRotator, SpawnInfo);

		RemoveWeapon(CurrentWeapon);
	}
}

void ALabyrinthCharacter::ServerDropWeapon_Implementation()
{
	DropWeapon();
}

bool ALabyrinthCharacter::ServerDropWeapon_Validate()
{
	return true;
}

void ALabyrinthCharacter::OnEquipPrimaryWeapon()
{
	if (Inventory.Num() >= 1)
	{
		// Find the first weapon that uses the primary slot
		for (int32 i = 0; i < Inventory.Num(); i++)
		{
			ALWeapon* Weapon = Inventory[i];
			if (Weapon->GetStorageSlot() == EInventorySlot::Primary)
			{
				EquipWeapon(Weapon);
			}
		}
	}
}

void ALabyrinthCharacter::OnEquipSecondaryWeapon()
{
	if (Inventory.Num() >= 1)
	{
		// Find the first weapon that uses the secondary slot
		for (int32 i = 0; i < Inventory.Num(); i++)
		{
			ALWeapon* Weapon = Inventory[i];
			if (Weapon->GetStorageSlot() == EInventorySlot::Secondary)
			{
				EquipWeapon(Weapon);
			}
		}
	}
}

bool ALabyrinthCharacter::WeaponSlotAvailable(EInventorySlot CheckSlot)
{
	// special find function used instead of looping the array and using if conditions.
	// [=] prefix means "capture by value"
	// [] means "capture nothing"
	// [&] means "capture by reference

	return nullptr == Inventory.FindByPredicate([=](ALWeapon* W){ return W->GetStorageSlot() == CheckSlot; });
}

void ALabyrinthCharacter::AddWeapon(ALWeapon* Weapon)
{
	if (Weapon && Role == ROLE_Authority)
	{
		Weapon->OnEnterInventory(this);
		Inventory.AddUnique(Weapon);

		// Equip first weapon in inventory
		if (Inventory.Num() > 0)
			EquipWeapon(Inventory[0]);
	}
}

void ALabyrinthCharacter::RemoveWeapon(ALWeapon* Weapon)
{
	if (Weapon && Role == ROLE_Authority)
	{
		bool bIsCurrent = CurrentWeapon == Weapon;

		if (Inventory.Contains(Weapon))
		{
			Weapon->OnLeaveInventory();
		}
		Inventory.RemoveSingle(Weapon);

		// Replace weapon if we removed our current weapon
		if (bIsCurrent && Inventory.Num() > 0)
			SetCurrentWeapon(Inventory[0]);

		// Clear reference to weapon if we have no items left 
		if (Inventory.Num() == 0)
			SetCurrentWeapon(nullptr);
	}
}


void ALabyrinthCharacter::OnRep_CurrentWeapon(ALWeapon* LastWeapon)
{
	SetCurrentWeapon(CurrentWeapon, LastWeapon);
}

void ALabyrinthCharacter::ServerEquipWeapon_Implementation(ALWeapon* Weapon)
{
	EquipWeapon(Weapon);
}

bool ALabyrinthCharacter::ServerEquipWeapon_Validate(ALWeapon* Weapon)
{
	return true;
}

void ALabyrinthCharacter::EquipWeapon(ALWeapon* Weapon)
{
	if (Weapon)
	{
		if (Role == ROLE_Authority)
		{
			SetCurrentWeapon(Weapon);
		}
		else
		{
			ServerEquipWeapon(Weapon);
		}
	}
}

void ALabyrinthCharacter::SetCurrentWeapon(class ALWeapon* NewWeapon, class ALWeapon* LastWeapon)
{
	ALWeapon* LocalLastWeapon = nullptr;

	if (LastWeapon)
	{
		LocalLastWeapon = LastWeapon;
	}
	else if (NewWeapon != CurrentWeapon)
	{
		LocalLastWeapon = CurrentWeapon;
	}

	// Unequip the current weapon
	if (LocalLastWeapon)
	{
		LocalLastWeapon->OnUnEquip();
	}

	CurrentWeapon = NewWeapon;

	if (NewWeapon)
	{
		NewWeapon->SetOwningPawn(this);
		NewWeapon->OnEquip();
	}
}

void ALabyrinthCharacter::StopAllAnimMontages()
{
	USkeletalMeshComponent* UseMesh = GetMesh();
	if (UseMesh && UseMesh->AnimScriptInstance)
	{
		UseMesh->AnimScriptInstance->Montage_Stop(0.0f);
	}
}
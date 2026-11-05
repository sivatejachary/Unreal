// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.

#include "../Public/IUCharacter.h"
#include "../Public/IUCharacterRegistry.h"
#include "../../IUVehicles/Public/IUVehicleBase.h"

AIUCharacter::AIUCharacter()
{
    bReplicates = true;
    IUMovement = new UIUCharacterMovementComponent();
    IUMovement->Owner = this;
    CharacterMovement = IUMovement;

    // Initialize with default Character 001 (AAA_Female_Rigged)
    const auto* Def = UIUCharacterRegistry::Get()->GetDefaultCharacter();
    if (Def)
    {
        ActiveCharacterId = Def->CharacterId;
        Attributes.MaxHealth = Def->BaseHealth;
        Attributes.CurrentHealth = Def->BaseHealth;
        Attributes.MaxStamina = Def->BaseStamina;
        Attributes.CurrentStamina = Def->BaseStamina;
        IUMovement->WalkSpeed = Def->WalkSpeed;
        IUMovement->JogSpeed = Def->JogSpeed;
        IUMovement->SprintSpeed = Def->SprintSpeed;
        IUMovement->CrouchSpeed = Def->CrouchSpeed;
        IUMovement->ProneSpeed = Def->ProneSpeed;
        IUMovement->JumpZVelocity = Def->JumpZVelocity;
    }
}

AIUCharacter::~AIUCharacter()
{
    delete IUMovement;
    IUMovement = nullptr;
    CharacterMovement = nullptr;
}

void AIUCharacter::BeginPlay()
{
    ACharacter::BeginPlay();
}

void AIUCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    ACharacter::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AIUCharacter, ActiveCharacterId);
    DOREPLIFETIME(AIUCharacter, Attributes);
    DOREPLIFETIME(AIUCharacter, bIsSprinting);
    DOREPLIFETIME(AIUCharacter, bIsAiming);
    DOREPLIFETIME(AIUCharacter, bIsDead);
    DOREPLIFETIME(AIUCharacter, CurrentVehicle);
    DOREPLIFETIME(AIUCharacter, EquippedWeapon);
}

void AIUCharacter::Input_Move(const FInputActionValue& Value)
{
    if (bIsDead || CurrentVehicle != nullptr)
    {
        return;
    }
    const FVector Axis3D = Value.GetAxis3D();
    PendingMovementInput = Axis3D;
}

void AIUCharacter::Input_Look(const FInputActionValue& Value)
{
    if (bIsDead)
    {
        return;
    }
    const FVector2D LookDelta = Value.GetAxis2D();
    FRotator NewRot = GetActorRotation();
    NewRot.Yaw += LookDelta.X;
    NewRot.Pitch = FMath::Clamp(NewRot.Pitch + LookDelta.Y, -72.0f, 68.0f);
    SetActorRotation(NewRot);
}

void AIUCharacter::Input_StartSprint()
{
    if (bIsDead || bIsCrouched || Attributes.CurrentStamina <= 5.0f)
    {
        return;
    }
    bIsSprinting = true;
    bIsAiming = false;
    IUMovement->SetGait(EIULocomotionGait::Sprint, Attributes.CurrentStamina);
}

void AIUCharacter::Input_StopSprint()
{
    bIsSprinting = false;
    IUMovement->SetGait(bIsCrouched ? EIULocomotionGait::Crouch : EIULocomotionGait::Jog, Attributes.CurrentStamina);
}

void AIUCharacter::Input_ToggleCrouch()
{
    if (bIsDead || IUMovement->IsSwimming())
    {
        return;
    }
    if (bIsCrouched)
    {
        UnCrouch();
        IUMovement->SetGait(EIULocomotionGait::Jog, Attributes.CurrentStamina);
    }
    else
    {
        bIsSprinting = false;
        Crouch();
        IUMovement->SetGait(EIULocomotionGait::Crouch, Attributes.CurrentStamina);
    }
}

void AIUCharacter::Input_Jump()
{
    if (bIsDead || bIsCrouched || Attributes.CurrentStamina < 10.0f)
    {
        return;
    }
    if (IUMovement->IsMovingOnGround())
    {
        Attributes.CurrentStamina = FMath::Max(0.0f, Attributes.CurrentStamina - 10.0f);
        Jump();
    }
}

void AIUCharacter::Input_StartAim()
{
    if (bIsDead) return;
    bIsAiming = true;
    bIsSprinting = false;
    IUMovement->SetGait(EIULocomotionGait::Walk, Attributes.CurrentStamina);
}

void AIUCharacter::Input_StopAim()
{
    bIsAiming = false;
    if (!bIsCrouched)
    {
        IUMovement->SetGait(EIULocomotionGait::Jog, Attributes.CurrentStamina);
    }
}

bool AIUCharacter::Input_Interact(AActor* TargetInteractable)
{
    if (bIsDead || TargetInteractable == nullptr)
    {
        return false;
    }
    const float Dist = FVector::Dist(GetActorLocation(), TargetInteractable->GetActorLocation());
    return Dist <= 350.0f; // 3.5m interaction radius
}

void AIUCharacter::Tick(float DeltaSeconds)
{
    ACharacter::Tick(DeltaSeconds);
    if (bIsDead || CurrentVehicle != nullptr)
    {
        return;
    }

    // Update stamina drain / regeneration
    const bool bMoving = PendingMovementInput.Size2D() > 0.1f;
    if (bIsSprinting && bMoving)
    {
        Attributes.CurrentStamina = FMath::Max(0.0f, Attributes.CurrentStamina - (Attributes.StaminaDrainPerSecond * DeltaSeconds));
        if (Attributes.CurrentStamina <= 0.0f)
        {
            Input_StopSprint();
        }
    }
    else
    {
        Attributes.CurrentStamina = FMath::Min(Attributes.MaxStamina, Attributes.CurrentStamina + (Attributes.StaminaRegenPerSecond * DeltaSeconds));
    }

    IUMovement->SimulateMovementStep(PendingMovementInput, DeltaSeconds, Attributes.CurrentStamina);
}

float AIUCharacter::ServerApplyDamage(float RawDamage, bool bIsHeadshot, AActor* DamageCauser, float MaxValidDistance)
{
    if (!HasAuthority() || bIsDead || RawDamage <= 0.0f)
    {
        return 0.0f;
    }

    // Server-side distance validation against impossible long-range hit injection
    if (DamageCauser != nullptr)
    {
        const float HitDistance = FVector::Dist(GetActorLocation(), DamageCauser->GetActorLocation());
        if (HitDistance > MaxValidDistance)
        {
            return 0.0f; // Rejected by server validation
        }
    }

    float EffectiveDamage = bIsHeadshot ? (RawDamage * 2.0f) : RawDamage;

    // Armor absorbs 60% of incoming body damage (30% on headshot)
    if (Attributes.CurrentArmor > 0.0f)
    {
        const float AbsorptionRatio = bIsHeadshot ? 0.30f : 0.60f;
        const float ArmorDamage = FMath::Min(Attributes.CurrentArmor, EffectiveDamage * AbsorptionRatio);
        Attributes.CurrentArmor -= ArmorDamage;
        EffectiveDamage -= ArmorDamage;
    }

    Attributes.CurrentHealth = FMath::Max(0.0f, Attributes.CurrentHealth - EffectiveDamage);

    if (Attributes.CurrentHealth <= 0.0f)
    {
        bIsDead = true;
        bIsSprinting = false;
        IUMovement->Velocity = FVector::ZeroVector();

        if (auto* MyPS = static_cast<AIUPlayerState*>(GetPlayerState()))
        {
            MyPS->ServerRecordDeath();
        }
        if (auto* AttackerChar = dynamic_cast<AIUCharacter*>(DamageCauser))
        {
            if (auto* AttackerPS = static_cast<AIUPlayerState*>(AttackerChar->GetPlayerState()))
            {
                const int32 VictimId = GetPlayerState() ? GetPlayerState()->GetPlayerId() : -1;
                AttackerPS->ServerRecordKill(VictimId);
                AttackerPS->ServerAddPoliceHeat(45.0f);
            }
        }
    }

    return EffectiveDamage;
}

void AIUCharacter::ServerHeal(float HealAmount)
{
    if (HasAuthority() && !bIsDead && HealAmount > 0.0f)
    {
        Attributes.CurrentHealth = FMath::Min(Attributes.MaxHealth, Attributes.CurrentHealth + HealAmount);
    }
}

void AIUCharacter::ServerRestoreArmor(float ArmorAmount)
{
    if (HasAuthority() && !bIsDead && ArmorAmount > 0.0f)
    {
        Attributes.CurrentArmor = FMath::Min(Attributes.MaxArmor, Attributes.CurrentArmor + ArmorAmount);
    }
}

void AIUCharacter::ServerRespawnAtTransform(const FTransform& SpawnTransform)
{
    if (!HasAuthority())
    {
        return;
    }
    bIsDead = false;
    Attributes.CurrentHealth = Attributes.MaxHealth;
    Attributes.CurrentStamina = Attributes.MaxStamina;
    Attributes.CurrentArmor = 50.0f;
    SetActorLocation(SpawnTransform.Translation);
    SetActorRotation(SpawnTransform.Rotation);
    IUMovement->Velocity = FVector::ZeroVector();
    IUMovement->SetMovementMode(MOVE_Walking);
}

bool AIUCharacter::ServerEnterVehicle(AIUVehicleBase* VehicleToEnter)
{
    if (!HasAuthority() || bIsDead || VehicleToEnter == nullptr || CurrentVehicle != nullptr)
    {
        return false;
    }
    if (VehicleToEnter->ServerAttemptPossessDriver(this))
    {
        CurrentVehicle = VehicleToEnter;
        if (auto* PC = dynamic_cast<AIUPlayerController*>(GetController()))
        {
            PC->SetVehicleInputContext(true);
        }
        return true;
    }
    return false;
}

bool AIUCharacter::ServerExitVehicle()
{
    if (!HasAuthority() || CurrentVehicle == nullptr)
    {
        return false;
    }
    AIUVehicleBase*ExitingVehicle = CurrentVehicle;
    if (ExitingVehicle->ServerReleaseDriver(this))
    {
        CurrentVehicle = nullptr;
        if (auto* PC = dynamic_cast<AIUPlayerController*>(GetController()))
        {
            PC->SetVehicleInputContext(false);
        }
        return true;
    }
    return false;
}

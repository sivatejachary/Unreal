// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.

#include "../Public/IUCharacterMovementComponent.h"

UIUCharacterMovementComponent::UIUCharacterMovementComponent()
{
    SetIsReplicatedByDefault(true);
    MaxWalkSpeed = BaseJogSpeed;
    MaxWalkSpeedCrouched = CrouchWalkSpeed;
    MaxSwimSpeed = 320.0f;
    JumpZVelocity = 500.0f;
    AirControl = 0.40f;
    Buoyancy = 1.08f;
    bOrientRotationToMovement = true;
}

void UIUCharacterMovementComponent::SetGait(EIULocomotionGait NewGait, float AvailableStamina)
{
    if (NewGait == EIULocomotionGait::Sprint && AvailableStamina <= 5.0f)
    {
        CurrentGait = EIULocomotionGait::Jog;
    }
    else
    {
        CurrentGait = NewGait;
    }
    MaxWalkSpeed = GetTargetMaxSpeed(AvailableStamina);
}

float UIUCharacterMovementComponent::GetTargetMaxSpeed(float AvailableStamina) const
{
    if (IsSwimming())
    {
        return (CurrentGait == EIULocomotionGait::Sprint && AvailableStamina > 5.0f) ? SwimFastSpeed : MaxSwimSpeed;
    }

    switch (CurrentGait)
    {
        case EIULocomotionGait::Walk:
            return BaseWalkSpeed;
        case EIULocomotionGait::Jog:
            return BaseJogSpeed;
        case EIULocomotionGait::Sprint:
            return (AvailableStamina > 0.0f) ? SprintSpeed : BaseJogSpeed;
        case EIULocomotionGait::Crouch:
            return CrouchWalkSpeed;
        case EIULocomotionGait::SwimSurface:
        case EIULocomotionGait::SwimDive:
            return MaxSwimSpeed;
        default:
            return BaseJogSpeed;
    }
}

void UIUCharacterMovementComponent::UpdateWaterState(float ActorZ, float WaterLevelZ)
{
    WaterSurfaceZ = WaterLevelZ;
    ImmersionDepth = FMath::Max(0.0f, WaterLevelZ - ActorZ);

    // Transition into Swimming when submerged deeper than 115 cm (chest depth)
    if (ImmersionDepth >= 115.0f)
    {
        if (MovementMode != MOVE_Swimming)
        {
            SetMovementMode(MOVE_Swimming);
            CurrentGait = EIULocomotionGait::SwimSurface;
        }
    }
    else if (MovementMode == MOVE_Swimming && ImmersionDepth < 85.0f)
    {
        SetMovementMode(MOVE_Walking);
        CurrentGait = EIULocomotionGait::Jog;
    }
}

void UIUCharacterMovementComponent::SimulateMovementStep(const FVector& InputDirection, float DeltaSeconds, float AvailableStamina)
{
    if (DeltaSeconds <= 0.0f)
    {
        return;
    }

    const FVector ClampedDir = InputDirection.GetSafeNormal();
    const float TargetSpeed = GetTargetMaxSpeed(AvailableStamina);
    MaxWalkSpeed = TargetSpeed;

    Velocity.X = ClampedDir.X * TargetSpeed;
    Velocity.Y = ClampedDir.Y * TargetSpeed;

    if (IsSwimming())
    {
        // Apply buoyancy and vertical swim control
        Velocity.Z = ClampedDir.Z * (TargetSpeed * 0.65f);
    }
    else if (IsFalling())
    {
        Velocity.Z += (-980.0f * GravityScale) * DeltaSeconds;
    }

    if (Owner)
    {
        FVector NewLocation = Owner->GetActorLocation() + (Velocity * DeltaSeconds);
        if (!IsSwimming() && NewLocation.Z <= 0.0f && Velocity.Z < 0.0f)
        {
            NewLocation.Z = 0.0f;
            Velocity.Z = 0.0f;
            SetMovementMode(MOVE_Walking);
        }
        Owner->SetActorLocation(NewLocation);
    }
}

bool UIUCharacterMovementComponent::ValidateClientReportedVelocity(const FVector& ReportedVelocity, float ToleranceMultiplier) const
{
    const float HorizontalSpeed = ReportedVelocity.Size2D();
    const float MaxAllowed = SprintSpeed * ToleranceMultiplier;
    return HorizontalSpeed <= MaxAllowed;
}

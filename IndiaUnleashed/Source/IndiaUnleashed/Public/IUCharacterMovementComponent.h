// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.
// Custom Character Movement Component supporting Walking, Sprinting, Crouching,
// Jumping, and Swimming (Hussain Sagar Lake & Musi River water volumes) with
// server-authoritative stamina consumption and speed validation.

#pragma once

#include "IUUnrealCoreCompat.h"

UENUM(BlueprintType)
enum class EIULocomotionGait : uint8
{
    Walk = 0,
    Jog = 1,
    Sprint = 2,
    Crouch = 3,
    SwimSurface = 4,
    SwimDive = 5
};

UCLASS(ClassGroup=(IndiaUnleashed), meta=(BlueprintSpawnableComponent))
class INDIAUNLEASHED_API UIUCharacterMovementComponent : public UCharacterMovementComponent
{
    GENERATED_BODY()

public:
    UIUCharacterMovementComponent();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="IU|Movement")
    float BaseWalkSpeed = 240.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="IU|Movement")
    float BaseJogSpeed = 500.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="IU|Movement")
    float SprintSpeed = 760.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="IU|Movement")
    float CrouchWalkSpeed = 260.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="IU|Movement")
    float SwimFastSpeed = 420.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="IU|Movement")
    float WaterSurfaceZ = -120.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IU|Movement")
    EIULocomotionGait CurrentGait = EIULocomotionGait::Jog;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IU|Movement")
    float ImmersionDepth = 0.0f;

    void SetGait(EIULocomotionGait NewGait, float AvailableStamina);
    EIULocomotionGait GetCurrentGait() const { return CurrentGait; }

    float GetTargetMaxSpeed(float AvailableStamina) const;
    void UpdateWaterState(float ActorZ, float WaterLevelZ);
    void SimulateMovementStep(const FVector& InputDirection, float DeltaSeconds, float AvailableStamina);
    bool ValidateClientReportedVelocity(const FVector& ReportedVelocity, float ToleranceMultiplier = 1.15f) const;
};

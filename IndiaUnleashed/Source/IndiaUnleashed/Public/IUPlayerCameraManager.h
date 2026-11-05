// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.
// Third-Person Dynamic Camera Manager supporting smooth FOV interpolation
// between Exploration, Sprinting, Combat ADS, and High-Speed Vehicle Driving.

#pragma once

#include "IUUnrealCoreCompat.h"

UENUM(BlueprintType)
enum class EIUCameraMode : uint8
{
    ThirdPersonExploration = 0,
    ThirdPersonSprint = 1,
    CombatAimDownSights = 2,
    VehicleFollow = 3
};

UCLASS()
class INDIAUNLEASHED_API AIUPlayerCameraManager : public APlayerCameraManager
{
    GENERATED_BODY()

public:
    AIUPlayerCameraManager();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="IU|Camera")
    float ExplorationFOV = 85.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="IU|Camera")
    float SprintFOV = 96.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="IU|Camera")
    float AimDownSightsFOV = 60.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="IU|Camera")
    float VehicleBaseFOV = 88.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="IU|Camera")
    float VehicleMaxFOV = 104.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="IU|Camera")
    float FOVInterpSpeed = 10.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="IU|Camera")
    FVector ThirdPersonSocketOffset{0.0f, 55.0f, 65.0f};

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="IU|Camera")
    float TargetArmLength = 285.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IU|Camera")
    EIUCameraMode ActiveCameraMode = EIUCameraMode::ThirdPersonExploration;

    void SetCameraMode(EIUCameraMode NewMode);
    void UpdateDynamicCamera(float DeltaTime, float CurrentSpeedCmPerSec);
    FRotator ClampCameraRotation(const FRotator& InputRotation) const;
};

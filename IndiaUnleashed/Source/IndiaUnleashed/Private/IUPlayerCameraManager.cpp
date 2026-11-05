// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.

#include "../Public/IUPlayerCameraManager.h"

AIUPlayerCameraManager::AIUPlayerCameraManager()
{
    DefaultFOV = ExplorationFOV;
    CurrentFOV = ExplorationFOV;
    ViewPitchMin = -72.0f;
    ViewPitchMax = 68.0f;
}

void AIUPlayerCameraManager::SetCameraMode(EIUCameraMode NewMode)
{
    ActiveCameraMode = NewMode;
    switch (NewMode)
    {
        case EIUCameraMode::ThirdPersonExploration:
            TargetArmLength = 285.0f;
            break;
        case EIUCameraMode::ThirdPersonSprint:
            TargetArmLength = 320.0f;
            break;
        case EIUCameraMode::CombatAimDownSights:
            TargetArmLength = 155.0f;
            break;
        case EIUCameraMode::VehicleFollow:
            TargetArmLength = 460.0f;
            break;
    }
}

void AIUPlayerCameraManager::UpdateDynamicCamera(float DeltaTime, float CurrentSpeedCmPerSec)
{
    float TargetFOV = ExplorationFOV;
    switch (ActiveCameraMode)
    {
        case EIUCameraMode::ThirdPersonExploration:
            TargetFOV = ExplorationFOV;
            break;
        case EIUCameraMode::ThirdPersonSprint:
            TargetFOV = SprintFOV;
            break;
        case EIUCameraMode::CombatAimDownSights:
            TargetFOV = AimDownSightsFOV;
            break;
        case EIUCameraMode::VehicleFollow:
        {
            const float NormalizedSpeed = FMath::Clamp(CurrentSpeedCmPerSec / 3500.0f, 0.0f, 1.0f);
            TargetFOV = VehicleBaseFOV + (VehicleMaxFOV - VehicleBaseFOV) * NormalizedSpeed;
            break;
        }
    }

    CurrentFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, DeltaTime, FOVInterpSpeed);
}

FRotator AIUPlayerCameraManager::ClampCameraRotation(const FRotator& InputRotation) const
{
    FRotator OutRot = InputRotation;
    OutRot.Pitch = FMath::Clamp(OutRot.Pitch, ViewPitchMin, ViewPitchMax);
    while (OutRot.Yaw >= 360.0f) OutRot.Yaw -= 360.0f;
    while (OutRot.Yaw < 0.0f) OutRot.Yaw += 360.0f;
    return OutRot;
}

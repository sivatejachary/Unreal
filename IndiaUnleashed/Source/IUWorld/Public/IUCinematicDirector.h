// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.
// Step 4 & 5: Unreal Engine Sequencer Cinematic Director & Hyderabad Environment Upgrade.
// Keeps Cinematic Camera evaluation strictly decoupled from Gameplay Camera logic.

#pragma once

#include "IUHyderabadTypes.h"

USTRUCT(BlueprintType)
struct FIUCinematicShotDefinition
{
    GENERATED_BODY()

    FString ShotName;
    float StartTimeSec = 0.0f;
    float EndTimeSec = 4.0f;
    FVector StartCameraLocation{0.0f, 0.0f, 0.0f};
    FVector EndCameraLocation{0.0f, 0.0f, 0.0f};
    FVector LookAtTargetLocation{0.0f, 0.0f, 0.0f};
    float FocalLengthMm = 35.0f;     // 24mm Wide, 35mm Anamorphic, 85mm Portrait Close-up
    float ApertureFStop = 2.8f;      // Controls Depth of Field bokeh blur
    float MotionBlurAmount = 0.5f;
    FString ColorGradingPreset = "ACES_DeccanGoldenHour";
    FString SubtitleDialogue;
};

USTRUCT(BlueprintType)
struct FIUCinematicCameraFrame
{
    GENERATED_BODY()

    bool bCinematicActive = false;
    FString ActiveShotName;
    FVector CameraLocation{0.0f, 0.0f, 0.0f};
    FRotator CameraRotation{0.0f, 0.0f, 0.0f};
    float CurrentFocalLengthMm = 35.0f;
    float CurrentApertureFStop = 2.8f;
    float GameplayHandoffBlendAlpha = 0.0f; // 0.0 = Pure Sequencer, 1.0 = Seamless Handoff to Gameplay Camera
};

USTRUCT(BlueprintType)
struct FIUEnvironmentUpgradeState
{
    GENERATED_BODY()

    bool bStreetlampsIlluminated = false;
    float RoadSurfaceRoughness = 0.85f;     // Drops to 0.16 during Monsoon rain for wet road reflections
    float PlanarReflectionIntensity = 0.25f;// Increases to 0.90 during Monsoon rain
    int32 ActiveRainParticlesPerCell = 0;
    int32 ActiveGoldenHourDustMotes = 250;
};

UCLASS()
class IUWORLD_API UIUCinematicDirectorSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    UIUCinematicDirectorSubsystem();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IU|Cematics")
    TArray<FIUCinematicShotDefinition> HyderabadIntroSequence;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IU|Environment")
    FIUEnvironmentUpgradeState EnvironmentState;

    void InitializeHyderabadIntroSequence();
    void StartCinematicIntro();
    void StopCinematicIntro();
    FIUCinematicCameraFrame EvaluateCinematicTime(float SequenceTimeSeconds, const FVector& GameplayCameraLocation, const FRotator& GameplayCameraRotation);

    // Step 5 Environment Upgrade Updater
    void UpdateHyderabadEnvironmentEffects(float TimeOfDayHours, bool bIsMonsoonRain);

private:
    bool bIsPlaying = false;
};

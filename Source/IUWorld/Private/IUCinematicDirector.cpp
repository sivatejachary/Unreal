// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.

#include "../Public/IUCinematicDirector.h"

UIUCinematicDirectorSubsystem::UIUCinematicDirectorSubsystem()
{
    InitializeHyderabadIntroSequence();
}

void UIUCinematicDirectorSubsystem::InitializeHyderabadIntroSequence()
{
    HyderabadIntroSequence.clear();

    // Shot 1: Wide Establishing Aerial Shot over Hussain Sagar & 18m Buddha Statue (24mm Wide Lens)
    HyderabadIntroSequence.push_back({
        "Shot01_HussainSagarAerialEstablish",
        0.0f, 4.0f,
        FVector(-9500.0f, -195000.0f, 4200.0f),
        FVector( 4500.0f, -165000.0f, 2800.0f),
        FVector(    0.0f, -210000.0f, 1800.0f),
        24.0f, 5.6f, 0.45f, "ACES_TealAndGolden",
        "HYDERABAD — Where four centuries of heritage meet the silicon frontier."
    });

    // Shot 2: Low-Angle Dolly Tracking Shot through Laad Bazaar into Charminar Arches (35mm Lens)
    HyderabadIntroSequence.push_back({
        "Shot02_CharminarArchDollyTrack",
        4.0f, 8.0f,
        FVector(-6500.0f, 200.0f, 320.0f),
        FVector(-1200.0f,  50.0f, 450.0f),
        FVector(    0.0f,   0.0f, 1800.0f),
        35.0f, 2.8f, 0.55f, "ACES_DeccanGoldenHour",
        "Beneath the four minarets of the Old City, every street corner tells a story."
    });

    // Shot 3: 85mm Portrait Close-Up on Protagonist with Shallow DOF (f/1.4), Eye Tracking & Lip Sync
    HyderabadIntroSequence.push_back({
        "Shot03_HeroCharacterCloseUp",
        8.0f, 12.0f,
        FVector(-250.0f, 1450.0f, 175.0f),
        FVector(-120.0f, 1380.0f, 178.0f),
        FVector(   0.0f, 1600.0f, 172.0f),
        85.0f, 1.4f, 0.25f, "ACES_CinematicPortrait",
        "They thought they owned these streets. Tonight, we take them back."
    });

    // Shot 4: Dynamic Tracking Sweep across Durgam Cheruvu Cable Bridge & Cyber Towers (50mm Lens)
    HyderabadIntroSequence.push_back({
        "Shot04_CyberCityCableBridgeSweep",
        12.0f, 16.0f,
        FVector(-165000.0f, -155000.0f, 2400.0f),
        FVector(-215000.0f, -180000.0f, 3600.0f),
        FVector(-220000.0f, -185000.0f, 2200.0f),
        50.0f, 2.8f, 0.65f, "ACES_CyberNeonContrast",
        "From HITEC City to the ramparts of Golconda — the city is yours."
    });

    // Shot 5: Smooth Cubic Handoff Transition into Third-Person Gameplay Camera
    HyderabadIntroSequence.push_back({
        "Shot05_SeamlessGameplayCameraHandoff",
        16.0f, 18.0f,
        FVector(-1800.0f, 2200.0f, 850.0f),
        FVector(    0.0f, 2500.0f, 420.0f),
        FVector(    0.0f, 1600.0f, 180.0f),
        35.0f, 4.0f, 0.35f, "ACES_GameplayNeutral",
        "INDIA UNLEASHED — Gameplay Active."
    });
}

void UIUCinematicDirectorSubsystem::StartCinematicIntro()
{
    bIsPlaying = true;
}

void UIUCinematicDirectorSubsystem::StopCinematicIntro()
{
    bIsPlaying = false;
}

FIUCinematicCameraFrame UIUCinematicDirectorSubsystem::EvaluateCinematicTime(
    float SequenceTimeSeconds,
    const FVector& GameplayCameraLocation,
    const FRotator& GameplayCameraRotation)
{
    FIUCinematicCameraFrame OutFrame;
    if (!bIsPlaying || SequenceTimeSeconds < 0.0f || SequenceTimeSeconds >= 18.0f)
    {
        bIsPlaying = false;
        OutFrame.bCinematicActive = false;
        OutFrame.CameraLocation = GameplayCameraLocation;
        OutFrame.CameraRotation = GameplayCameraRotation;
        OutFrame.GameplayHandoffBlendAlpha = 1.0f;
        return OutFrame;
    }

    OutFrame.bCinematicActive = true;

    for (const auto& Shot : HyderabadIntroSequence)
    {
        if (SequenceTimeSeconds >= Shot.StartTimeSec && SequenceTimeSeconds <= Shot.EndTimeSec)
        {
            OutFrame.ActiveShotName = Shot.ShotName;
            const float Duration = FMath::Max(0.01f, Shot.EndTimeSec - Shot.StartTimeSec);
            const float RawAlpha = FMath::Clamp((SequenceTimeSeconds - Shot.StartTimeSec) / Duration, 0.0f, 1.0f);
            // Smoothstep cubic easing: 3t^2 - 2t^3
            const float SmoothAlpha = RawAlpha * RawAlpha * (3.0f - 2.0f * RawAlpha);

            OutFrame.CameraLocation = Shot.StartCameraLocation + (Shot.EndCameraLocation - Shot.StartCameraLocation) * SmoothAlpha;
            OutFrame.CurrentFocalLengthMm = Shot.FocalLengthMm;
            OutFrame.CurrentApertureFStop = Shot.ApertureFStop;

            const FVector LookDir = (Shot.LookAtTargetLocation - OutFrame.CameraLocation).GetSafeNormal();
            const float YawDeg = std::atan2(LookDir.Y, LookDir.X) * (180.0f / 3.1415926535f);
            const float PitchDeg = std::asin(FMath::Clamp(LookDir.Z, -1.0f, 1.0f)) * (180.0f / 3.1415926535f);
            OutFrame.CameraRotation = FRotator(PitchDeg, YawDeg, 0.0f);

            if (Shot.ShotName == "Shot05_SeamlessGameplayCameraHandoff")
            {
                OutFrame.GameplayHandoffBlendAlpha = SmoothAlpha;
                OutFrame.CameraLocation = OutFrame.CameraLocation * (1.0f - SmoothAlpha) + GameplayCameraLocation * SmoothAlpha;
            }
            break;
        }
    }

    return OutFrame;
}

void UIUCinematicDirectorSubsystem::UpdateHyderabadEnvironmentEffects(float TimeOfDayHours, bool bIsMonsoonRain)
{
    EnvironmentState.bStreetlampsIlluminated = (TimeOfDayHours >= 18.0f || TimeOfDayHours <= 6.0f);

    if (bIsMonsoonRain)
    {
        EnvironmentState.RoadSurfaceRoughness = 0.16f;      // Wet asphalt mirror-like reflections
        EnvironmentState.PlanarReflectionIntensity = 0.90f; // Strong puddle reflections
        EnvironmentState.ActiveRainParticlesPerCell = 1800;
        EnvironmentState.ActiveGoldenHourDustMotes = 0;
    }
    else
    {
        EnvironmentState.RoadSurfaceRoughness = 0.85f;
        EnvironmentState.PlanarReflectionIntensity = 0.25f;
        EnvironmentState.ActiveRainParticlesPerCell = 0;
        EnvironmentState.ActiveGoldenHourDustMotes = (TimeOfDayHours >= 16.0f && TimeOfDayHours <= 18.2f) ? 400 : 120;
    }
}

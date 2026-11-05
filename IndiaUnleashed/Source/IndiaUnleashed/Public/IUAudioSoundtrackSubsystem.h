// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.
// Step 6: Original Soundtrack & Dynamic Music Stem Subsystem.

#pragma once

#include "IUUnrealCoreCompat.h"

UENUM(BlueprintType)
enum class EIUMusicStemState : uint8
{
    MainTheme = 0,
    Exploration = 1,
    Cinematic = 2,
    CombatAction = 3,
    VehicleDriving = 4,
    EmotionalStory = 5,
    NightAtmosphere = 6,
    EnvironmentalAmbience = 7
};

USTRUCT(BlueprintType)
struct FIUSoundtrackTrackDescriptor
{
    GENERATED_BODY()

    EIUMusicStemState StemState = EIUMusicStemState::MainTheme;
    FString TrackId;
    FString RelativeWavPath;
    float TempoBPM = 108.0f;
    FString RagaScaleInspiration;
};

UCLASS()
class INDIAUNLEASHED_API UIUAudioSoundtrackSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UIUAudioSoundtrackSubsystem();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IU|Audio")
    TArray<FIUSoundtrackTrackDescriptor> SoundtrackCatalog;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IU|Audio")
    EIUMusicStemState ActiveStem = EIUMusicStemState::MainTheme;

    void InitializeSoundtrackCatalog();
    const FIUSoundtrackTrackDescriptor* TransitionToMusicState(EIUMusicStemState NewState);
    bool VerifyAllWavAssetsOnDisk(const FString& ProjectRootDir) const;
};

// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.
// Server-Authoritative Mission Progression Subsystem.

#pragma once

#include "../../IndiaUnleashed/Public/IUUnrealCoreCompat.h"
#include "../../IndiaUnleashed/Public/IUPlayerState.h"

UENUM(BlueprintType)
enum class EIUMissionState : uint8
{
    NotStarted = 0,
    InProgress = 1,
    Completed = 2,
    Failed = 3
};

USTRUCT(BlueprintType)
struct FIUMissionObjective
{
    GENERATED_BODY()

    FString ObjectiveId;
    FString Description;
    FVector TargetLocation{0.0f, 0.0f, 0.0f};
    float CompletionRadiusCm = 1500.0f;
    bool bIsCompleted = false;
};

USTRUCT(BlueprintType)
struct FIUMissionDefinition
{
    GENERATED_BODY()

    FString MissionId;
    FString Title;
    int64 RewardCurrencyINR = 15000;
    int64 RewardXP = 1200;
    EIUMissionState State = EIUMissionState::NotStarted;
    int32 CurrentObjectiveIndex = 0;
    TArray<FIUMissionObjective> Objectives;
};

UCLASS()
class IUMISSIONS_API UIUMissionSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UIUMissionSubsystem();

    void RegisterDefaultHyderabadMissions();
    bool ServerStartMission(const FString& MissionId);
    bool ServerUpdateMissionProgress(const FString& MissionId, const FVector& PlayerLocation, AIUPlayerState* AuthoritativePlayerState);
    EIUMissionState GetMissionState(const FString& MissionId) const;
    const FIUMissionDefinition* GetMission(const FString& MissionId) const;

private:
    TMap<FString, FIUMissionDefinition> Missions;
};

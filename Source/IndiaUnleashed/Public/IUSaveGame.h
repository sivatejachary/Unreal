// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.
// Persistent Save/Load Progression system with integrity checksum validation.

#pragma once

#include "IUUnrealCoreCompat.h"

UCLASS()
class INDIAUNLEASHED_API UIUSaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    UPROPERTY(VisibleAnywhere, Category="IU|SaveGame")
    FString SaveSlotName = "IndiaUnleashed_Slot01";

    UPROPERTY(VisibleAnywhere, Category="IU|SaveGame")
    FString AccountId = "iu_acct_default";

    UPROPERTY(VisibleAnywhere, Category="IU|SaveGame")
    int64 SavedCurrencyINR = 5000;

    UPROPERTY(VisibleAnywhere, Category="IU|SaveGame")
    int64 SavedExperienceXP = 0;

    UPROPERTY(VisibleAnywhere, Category="IU|SaveGame")
    FVector LastPlayerLocation{-2200.0f, 450.0f, 110.0f};

    UPROPERTY(VisibleAnywhere, Category="IU|SaveGame")
    FString CurrentDistrict = "OldCity_Charminar";

    UPROPERTY(VisibleAnywhere, Category="IU|SaveGame")
    TArray<FString> OwnedVehicleIds;

    UPROPERTY(VisibleAnywhere, Category="IU|SaveGame")
    TArray<FString> CompletedMissionIds;

    uint64 ComputeIntegrityHash() const;
    bool SaveToDisk(const FString& FilePath) const;
    bool LoadFromDisk(const FString& FilePath);
};

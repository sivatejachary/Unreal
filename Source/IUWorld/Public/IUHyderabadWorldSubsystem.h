// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.
// Hyderabad Open-World Subsystem managing the 6 Districts, 18 Landmark POIs,
// Distance-Based NPC/Traffic Simulation Scaling, and Police Dispatch.

#pragma once

#include "IUHyderabadTypes.h"
#include "IUCharminarBlockoutActor.h"

UCLASS()
class IUWORLD_API UIUHyderabadWorldSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    UIUHyderabadWorldSubsystem();

    void InitializeHyderabadDistricts();

    const TArray<FIUHyderabadPOI>& GetAllPOIs() const { return POIRegistry; }
    TArray<FIUHyderabadPOI> GetPOIsByDistrict(EIUHyderabadDistrict District) const;
    const FIUHyderabadPOI* FindNearestPOI(const FVector& WorldLocation, float& OutDistanceCm) const;

    // Distance-based AI & Traffic simulation scaling (Rule 9: do not simulate every NPC at max detail)
    static EIUNPCSimulationTier EvaluateNPCSimulationTier(const FVector& NPCLocation, const TArray<FVector>& PlayerLocations);

    // Police AI Dispatch calculation based on Wanted Level (0-5)
    static int32 GetTargetPoliceUnitsForWantedLevel(int32 WantedLevel);

private:
    TArray<FIUHyderabadPOI> POIRegistry;
};

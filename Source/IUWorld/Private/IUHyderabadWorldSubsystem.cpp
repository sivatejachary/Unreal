// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.

#include "../Public/IUHyderabadWorldSubsystem.h"

UIUHyderabadWorldSubsystem::UIUHyderabadWorldSubsystem()
{
    InitializeHyderabadDistricts();
}

void UIUHyderabadWorldSubsystem::InitializeHyderabadDistricts()
{
    POIRegistry.clear();

    // District 1: Old City (Alpha 0.1 Playable District)
    POIRegistry.push_back({"POI_OLD_01", "Charminar Monument Plaza", EIUHyderabadDistrict::OldCity, FVector(0.0f, 0.0f, 0.0f), 6000.0f, true, "DL_OldCity_Core"});
    POIRegistry.push_back({"POI_OLD_02", "Laad Bazaar Bangle Market", EIUHyderabadDistrict::OldCity, FVector(-8500.0f, 200.0f, 0.0f), 4500.0f, true, "DL_OldCity_Core"});
    POIRegistry.push_back({"POI_OLD_03", "Char Kaman Archway", EIUHyderabadDistrict::OldCity, FVector(0.0f, -9500.0f, 0.0f), 4000.0f, true, "DL_OldCity_Core"});
    POIRegistry.push_back({"POI_OLD_04", "Mecca Masjid Courtyard", EIUHyderabadDistrict::OldCity, FVector(-1500.0f, 7200.0f, 0.0f), 5000.0f, true, "DL_OldCity_Core"});

    // District 2: Tank Bund District (Hussain Sagar Waterfront)
    POIRegistry.push_back({"POI_TB_01", "Hussain Sagar Lake & Buddha Statue Island", EIUHyderabadDistrict::TankBund, FVector(0.0f, -145000.0f, -120.0f), 15000.0f, true, "DL_TankBund"});
    POIRegistry.push_back({"POI_TB_02", "Tank Bund Embankment Promenade", EIUHyderabadDistrict::TankBund, FVector(18000.0f, -130000.0f, 200.0f), 8000.0f, true, "DL_TankBund"});
    POIRegistry.push_back({"POI_TB_03", "Necklace Road Street Racing Circuit", EIUHyderabadDistrict::TankBund, FVector(-22000.0f, -158000.0f, 150.0f), 9000.0f, false, "DL_TankBund"});

    // District 3: Cyber City (HITEC City & Gachibowli)
    POIRegistry.push_back({"POI_CYB_01", "Cyber Towers Quadrant", EIUHyderabadDistrict::CyberCity, FVector(-280000.0f, -210000.0f, 800.0f), 12000.0f, false, "DL_CyberCity"});
    POIRegistry.push_back({"POI_CYB_02", "Durgam Cheruvu Cable Bridge", EIUHyderabadDistrict::CyberCity, FVector(-230000.0f, -190000.0f, 650.0f), 14000.0f, false, "DL_CyberCity"});
    POIRegistry.push_back({"POI_CYB_03", "Gachibowli Financial Skyscraper Hub", EIUHyderabadDistrict::CyberCity, FVector(-340000.0f, -175000.0f, 950.0f), 15000.0f, false, "DL_CyberCity"});

    // District 4: Golconda District
    POIRegistry.push_back({"POI_GOL_01", "Golconda Fort Citadel & Bala Hisar", EIUHyderabadDistrict::Golconda, FVector(-190000.0f, -45000.0f, 3800.0f), 18000.0f, false, "DL_Golconda"});
    POIRegistry.push_back({"POI_GOL_02", "Deccan Granite Boulder Ridge", EIUHyderabadDistrict::Golconda, FVector(-215000.0f, -65000.0f, 2400.0f), 10000.0f, false, "DL_Golconda"});
    POIRegistry.push_back({"POI_GOL_03", "Qutb Shahi Heritage Gardens", EIUHyderabadDistrict::Golconda, FVector(-198000.0f, -82000.0f, 1100.0f), 9000.0f, false, "DL_Golconda"});

    // District 5: Airport District
    POIRegistry.push_back({"POI_AIR_01", "Rajiv Gandhi Inspired International Terminal", EIUHyderabadDistrict::AirportDistrict, FVector(-60000.0f, 320000.0f, 400.0f), 25000.0f, false, "DL_Airport"});
    POIRegistry.push_back({"POI_AIR_02", "PVNR Elevated Expressway Interchange", EIUHyderabadDistrict::AirportDistrict, FVector(-45000.0f, 140000.0f, 1200.0f), 12000.0f, false, "DL_Airport"});

    // District 6: Outer Hyderabad
    POIRegistry.push_back({"POI_OUT_01", "Pochampally-Inspired Handloom Village", EIUHyderabadDistrict::OuterHyderabad, FVector(210000.0f, 95000.0f, 300.0f), 11000.0f, false, "DL_OuterHyderabad"});
    POIRegistry.push_back({"POI_OUT_02", "Ananthagiri Deccan Forest Reserve", EIUHyderabadDistrict::OuterHyderabad, FVector(-410000.0f, 40000.0f, 4200.0f), 22000.0f, false, "DL_OuterHyderabad"});
    POIRegistry.push_back({"POI_OUT_03", "Outer Ring Road (ORR) Toll Plaza", EIUHyderabadDistrict::OuterHyderabad, FVector(165000.0f, 240000.0f, 500.0f), 10000.0f, false, "DL_OuterHyderabad"});
}

TArray<FIUHyderabadPOI> UIUHyderabadWorldSubsystem::GetPOIsByDistrict(EIUHyderabadDistrict District) const
{
    TArray<FIUHyderabadPOI> Filtered;
    for (const auto& P : POIRegistry)
    {
        if (P.District == District)
        {
            Filtered.push_back(P);
        }
    }
    return Filtered;
}

const FIUHyderabadPOI* UIUHyderabadWorldSubsystem::FindNearestPOI(const FVector& WorldLocation, float& OutDistanceCm) const
{
    const FIUHyderabadPOI* Best = nullptr;
    float BestDist = 1e30f;

    for (const auto& P : POIRegistry)
    {
        const float D = FVector::Dist(WorldLocation, P.WorldLocation);
        if (D < BestDist)
        {
            BestDist = D;
            Best = &P;
        }
    }
    OutDistanceCm = BestDist;
    return Best;
}

EIUNPCSimulationTier UIUHyderabadWorldSubsystem::EvaluateNPCSimulationTier(const FVector& NPCLocation, const TArray<FVector>& PlayerLocations)
{
    float MinDistCm = 1e30f;
    for (const auto& PLoc : PlayerLocations)
    {
        const float D = FVector::Dist(NPCLocation, PLoc);
        if (D < MinDistCm)
        {
            MinDistCm = D;
        }
    }

    if (MinDistCm <= 6000.0f) return EIUNPCSimulationTier::Tier0_FullBehaviorTreeAndPhysics; // <= 60m
    if (MinDistCm <= 15000.0f) return EIUNPCSimulationTier::Tier1_SimplifiedKinematicNav;    // <= 150m
    if (MinDistCm <= 35000.0f) return EIUNPCSimulationTier::Tier2_SplineCrowdDataOnly;       // <= 350m
    return EIUNPCSimulationTier::Tier3_CulledOrDormant;                                      // > 350m
}

int32 UIUHyderabadWorldSubsystem::GetTargetPoliceUnitsForWantedLevel(int32 WantedLevel)
{
    switch (FMath::Clamp(WantedLevel, 0, 5))
    {
        case 0: return 0;
        case 1: return 2;  // 2 Beat Patrol Officers
        case 2: return 4;  // 2 Police Interceptors (4 Officers)
        case 3: return 7;  // Roadblocks + 3 Interceptors
        case 4: return 12; // Tactical Van + Helicopter Spotter
        case 5: return 18; // Full City Lockdown & Armored Response
        default: return 0;
    }
}

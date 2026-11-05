// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.
// Fictionalized but geographically recognizable Hyderabad Open-World Map Data Types.

#pragma once

#include "../../IndiaUnleashed/Public/IUUnrealCoreCompat.h"

UENUM(BlueprintType)
enum class EIUHyderabadDistrict : uint8
{
    OldCity = 0,         // Charminar, Laad Bazaar, Mecca Masjid, Narrow Streets
    TankBund = 1,        // Hussain Sagar Lake, Buddha Statue, Necklace Road Waterfront
    CyberCity = 2,       // HITEC City, Cyber Towers, Gachibowli Skyscrapers & Overpasses
    Golconda = 3,        // Golconda Fort, Granite Hills, Ramparts & Exploration Ruins
    AirportDistrict = 4, // Shamshabad-inspired Terminal, PVNR Expressway, Cargo Apron
    OuterHyderabad = 5   // Telangana Villages, Deccan Farms, Forests & Rural Highways
};

UENUM(BlueprintType)
enum class EIUNPCSimulationTier : uint8
{
    Tier0_FullBehaviorTreeAndPhysics = 0, // < 60m from player
    Tier1_SimplifiedKinematicNav = 1,     // 60m - 150m
    Tier2_SplineCrowdDataOnly = 2,        // 150m - 350m
    Tier3_CulledOrDormant = 3             // > 350m
};

USTRUCT(BlueprintType)
struct FIUHyderabadPOI
{
    GENERATED_BODY()

    FString POIId;
    FString DisplayName;
    EIUHyderabadDistrict District = EIUHyderabadDistrict::OldCity;
    FVector WorldLocation{0.0f, 0.0f, 0.0f}; // Unreal units (cm) relative to Charminar origin
    FVector GeographicCoordinates{78.486671f, 17.385044f, 505.0f}; // WGS84 Lon, Lat, Height (m) for Cesium
    float DiscoveryRadiusCm = 5000.0f;
    bool bIsPlayableInAlpha01 = false;
    FString DataLayerName;
};

USTRUCT(BlueprintType)
struct FIUArchitecturalBoxPrimitive
{
    GENERATED_BODY()

    FString ComponentName;
    FVector RelativeLocation{0.0f, 0.0f, 0.0f};
    FVector BoxExtentCm{100.0f, 100.0f, 100.0f}; // Half-extents in cm
    FString MaterialSlotName;
    bool bHasCollision = true;
};

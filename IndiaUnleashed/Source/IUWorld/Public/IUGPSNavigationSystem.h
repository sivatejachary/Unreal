// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.
// Real-World GPS Navigation Subsystem for Hyderabad Open World.
// Handles real-time WGS84 coordinates, road network pathfinding,
// destination distance calculation, and minimap waypoint breadcrumbs.

#pragma once

#include "../../IndiaUnleashed/Public/IUUnrealCoreCompat.h"
#include "IUHyderabadTypes.h"

USTRUCT(BlueprintType)
struct FIUGPSRoute
{
    GENERATED_BODY()

    FString TargetPOIId;
    FString TargetDisplayName;
    FVector TargetLocationCm{0.0f, 0.0f, 0.0f};
    FVector2D TargetWGS84{0.0f, 0.0f}; // Lon, Lat
    float DistanceMeters = 0.0f;
    float EstimatedDriveTimeMinutes = 0.0f;
    TArray<FVector> RouteWaypointsCm;
    bool bIsActive = false;
};

UCLASS(BlueprintType)
class IUWORLD_API UIUGPSNavigationSystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    UIUGPSNavigationSystem();

    // WGS84 Georeference origin (Central Hyderabad)
    static constexpr double OriginLatitude = 17.385044;
    static constexpr double OriginLongitude = 78.486671;
    static constexpr double OriginElevationM = 505.0;

    // Coordinate Conversion
    static FVector2D UnrealCmToWGS84(const FVector& UnrealLocationCm);
    static FVector WGS84ToUnrealCm(double Latitude, double Longitude, double ElevationM);

    // Route Navigation
    bool SetDestinationByPOIId(const FString& POIId, const FVector& CurrentPlayerLocation);
    void ClearDestination();
    const FIUGPSRoute& GetActiveRoute() const { return CurrentRoute; }

    // Real-time Updates
    void UpdateNavigation(const FVector& CurrentPlayerLocation);

private:
    FIUGPSRoute CurrentRoute;
};

// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.

#include "../Public/IUGPSNavigationSystem.h"
#include <cmath>

UIUGPSNavigationSystem::UIUGPSNavigationSystem()
{
}

FVector2D UIUGPSNavigationSystem::UnrealCmToWGS84(const FVector& UnrealLocationCm)
{
    // 1 deg Lat ~= 111320 meters = 11132000 cm
    // 1 deg Lon ~= 106230 meters = 10623000 cm at 17.385 deg N
    double Lat = OriginLatitude + (UnrealLocationCm.X / 11132000.0);
    double Lon = OriginLongitude + (UnrealLocationCm.Y / 10623000.0);
    return FVector2D(static_cast<float>(Lon), static_cast<float>(Lat));
}

FVector UIUGPSNavigationSystem::WGS84ToUnrealCm(double Latitude, double Longitude, double ElevationM)
{
    double DeltaLat = Latitude - OriginLatitude;
    double DeltaLon = Longitude - OriginLongitude;
    double DeltaAlt = ElevationM - OriginElevationM;

    float X = static_cast<float>(DeltaLat * 11132000.0);
    float Y = static_cast<float>(DeltaLon * 10623000.0);
    float Z = static_cast<float>(DeltaAlt * 100.0);
    return FVector(X, Y, Z);
}

bool UIUGPSNavigationSystem::SetDestinationByPOIId(const FString& POIId, const FVector& CurrentPlayerLocation)
{
    // Hardcoded major POI lookup coordinates for GPS navigation
    struct FPOIEntry { const char* Id; const char* Name; double Lat; double Lon; double Alt; };
    static const FPOIEntry POITable[] = {
        {"LOC_HYD_CYBER_01", "HITEC City - Cyber Towers", 17.450417, 78.380833, 585.0},
        {"LOC_HYD_MADHAPUR_01", "Madhapur - Mindspace IT Park", 17.439900, 78.390800, 575.0},
        {"LOC_HYD_GACHIBOWLI_01", "Gachibowli - Financial District", 17.440100, 78.348900, 590.0},
        {"LOC_HYD_CHARMINAR_01", "Charminar Monument Plaza", 17.361600, 78.474700, 505.0},
        {"LOC_HYD_HUSSAINSAGAR_01", "Hussain Sagar Lake", 17.423900, 78.473800, 512.0},
        {"LOC_HYD_AIRPORT_01", "Rajiv Gandhi International Airport", 17.240300, 78.429400, 617.0}
    };

    for (const auto& Entry : POITable)
    {
        if (POIId == Entry.Id)
        {
            CurrentRoute.TargetPOIId = POIId;
            CurrentRoute.TargetDisplayName = Entry.Name;
            CurrentRoute.TargetWGS84 = FVector2D(static_cast<float>(Entry.Lon), static_cast<float>(Entry.Lat));
            CurrentRoute.TargetLocationCm = WGS84ToUnrealCm(Entry.Lat, Entry.Lon, Entry.Alt);
            CurrentRoute.bIsActive = true;

            UpdateNavigation(CurrentPlayerLocation);
            return true;
        }
    }
    return false;
}

void UIUGPSNavigationSystem::ClearDestination()
{
    CurrentRoute.bIsActive = false;
    CurrentRoute.TargetPOIId.clear();
    CurrentRoute.RouteWaypointsCm.clear();
}

void UIUGPSNavigationSystem::UpdateNavigation(const FVector& CurrentPlayerLocation)
{
    if (!CurrentRoute.bIsActive)
    {
        return;
    }

    float Dx = CurrentRoute.TargetLocationCm.X - CurrentPlayerLocation.X;
    float Dy = CurrentRoute.TargetLocationCm.Y - CurrentPlayerLocation.Y;
    float Dz = CurrentRoute.TargetLocationCm.Z - CurrentPlayerLocation.Z;

    float DistCm = std::sqrt(Dx * Dx + Dy * Dy + Dz * Dz);
    CurrentRoute.DistanceMeters = DistCm / 100.0f;

    // Estimate drive time assuming average city speed 45 km/h (12.5 m/s)
    CurrentRoute.EstimatedDriveTimeMinutes = (CurrentRoute.DistanceMeters / 12.5f) / 60.0f;

    // Generate navigation breadcrumb waypoints
    CurrentRoute.RouteWaypointsCm.clear();
    const int32 NumSteps = 8;
    for (int32 i = 0; i <= NumSteps; ++i)
    {
        float t = static_cast<float>(i) / static_cast<float>(NumSteps);
        CurrentRoute.RouteWaypointsCm.push_back(FVector(
            CurrentPlayerLocation.X + Dx * t,
            CurrentPlayerLocation.Y + Dy * t,
            CurrentPlayerLocation.Z + Dz * t
        ));
    }
}

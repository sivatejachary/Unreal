// Copyright 2020-2024 CesiumGS, Inc. and Contributors

#include "../Public/CesiumSunSky.h"
#include "../Public/CesiumGeoreference.h"
#include <cmath>

ACesiumSunSky::ACesiumSunSky()
{
    PrimaryActorTick.bCanEverTick = false;
    SolarTimeHours = 14.5f; // Deccan afternoon sunlight
    DayOfYear = 180;
    TimeZoneOffsetHours = 5.5f; // IST (India Standard Time)
}

void ACesiumSunSky::UpdateSunPosition()
{
    // Solar declination approximation
    float declinationDeg = 23.45f * std::sin((284 + DayOfYear) * (3.14159f / 365.0f));
    float solarHourAngleDeg = (SolarTimeHours - 12.0f) * 15.0f;

    double lat = Georeference ? Georeference->OriginLatitude : 17.385044;
    float latRad = static_cast<float>(lat * 3.14159265 / 180.0);
    float decRad = declinationDeg * 3.14159265f / 180.0f;
    float hraRad = solarHourAngleDeg * 3.14159265f / 180.0f;

    // Solar elevation angle
    float sinElevation = std::sin(latRad) * std::sin(decRad) + std::cos(latRad) * std::cos(decRad) * std::cos(hraRad);
    float elevationDeg = std::asin(std::max(-1.0f, std::min(1.0f, sinElevation))) * 180.0f / 3.14159265f;

    // Solar azimuth angle
    float cosAzimuth = (std::sin(decRad) - std::sin(latRad) * sinElevation) / (std::cos(latRad) * std::cos(std::asin(sinElevation)));
    float azimuthDeg = std::acos(std::max(-1.0f, std::min(1.0f, cosAzimuth))) * 180.0f / 3.14159265f;
    if (solarHourAngleDeg > 0)
    {
        azimuthDeg = 360.0f - azimuthDeg;
    }

    SetActorRotation(FRotator(elevationDeg, azimuthDeg, 0.0f));
}

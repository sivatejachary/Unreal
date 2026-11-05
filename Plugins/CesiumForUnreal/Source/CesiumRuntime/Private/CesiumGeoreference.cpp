// Copyright 2020-2024 CesiumGS, Inc. and Contributors

#include "../Public/CesiumGeoreference.h"
#include <cmath>

namespace CesiumWgs84
{
    static constexpr double WGS84_A = 6378137.0; // semi-major axis (meters)
    static constexpr double WGS84_F = 1.0 / 298.257223563; // flattening
    static constexpr double WGS84_B = 6356752.314245179; // semi-minor axis
    static constexpr double WGS84_E2 = 0.0066943799901413165; // first eccentricity squared
    static constexpr double DEG_TO_RAD = 3.14159265358979323846 / 180.0;
    static constexpr double RAD_TO_DEG = 180.0 / 3.14159265358979323846;

    static void GeodeticToEcef(double LonDeg, double LatDeg, double HeightM, double& OutX, double& OutY, double& OutZ)
    {
        double latRad = LatDeg * DEG_TO_RAD;
        double lonRad = LonDeg * DEG_TO_RAD;
        double sinLat = std::sin(latRad);
        double cosLat = std::cos(latRad);
        double sinLon = std::sin(lonRad);
        double cosLon = std::cos(lonRad);

        double n = WGS84_A / std::sqrt(1.0 - WGS84_E2 * sinLat * sinLat);

        OutX = (n + HeightM) * cosLat * cosLon;
        OutY = (n + HeightM) * cosLat * sinLon;
        OutZ = (n * (1.0 - WGS84_E2) + HeightM) * sinLat;
    }

    static void EcefToGeodetic(double X, double Y, double Z, double& OutLonDeg, double& OutLatDeg, double& OutHeightM)
    {
        double p = std::sqrt(X * X + Y * Y);
        double theta = std::atan2(Z * WGS84_A, p * WGS84_B);
        double sinTheta = std::sin(theta);
        double cosTheta = std::cos(theta);

        double latRad = std::atan2(
            Z + WGS84_E2 * (WGS84_A * WGS84_A / WGS84_B) * sinTheta * sinTheta * sinTheta,
            p - WGS84_E2 * WGS84_A * cosTheta * cosTheta * cosTheta
        );
        double lonRad = std::atan2(Y, X);

        double sinLat = std::sin(latRad);
        double n = WGS84_A / std::sqrt(1.0 - WGS84_E2 * sinLat * sinLat);

        OutHeightM = p / std::cos(latRad) - n;
        OutLatDeg = latRad * RAD_TO_DEG;
        OutLonDeg = lonRad * RAD_TO_DEG;
    }
}

ACesiumGeoreference::ACesiumGeoreference()
{
    PrimaryActorTick.bCanEverTick = false;

    // Default Central Hyderabad Origin
    OriginLatitude = 17.385044;  // Central Hyderabad (Abids / Nampally / Hussain Sagar south)
    OriginLongitude = 78.486671; // Central Hyderabad
    OriginHeight = 505.0;        // Deccan Plateau mean elevation
    Scale = 1.0;
}

void ACesiumGeoreference::SetOriginLongitudeLatitudeHeight(double InLongitude, double InLatitude, double InHeight)
{
    OriginLongitude = InLongitude;
    OriginLatitude = InLatitude;
    OriginHeight = InHeight;
}

FVector ACesiumGeoreference::TransformLongitudeLatitudeHeightToUnreal(const FVector& LongitudeLatitudeHeight) const
{
    double originX = 0.0, originY = 0.0, originZ = 0.0;
    CesiumWgs84::GeodeticToEcef(OriginLongitude, OriginLatitude, OriginHeight, originX, originY, originZ);

    double targetX = 0.0, targetY = 0.0, targetZ = 0.0;
    CesiumWgs84::GeodeticToEcef(LongitudeLatitudeHeight.X, LongitudeLatitudeHeight.Y, LongitudeLatitudeHeight.Z, targetX, targetY, targetZ);

    double dx = targetX - originX;
    double dy = targetY - originY;
    double dz = targetZ - originZ;

    double latRad = OriginLatitude * CesiumWgs84::DEG_TO_RAD;
    double lonRad = OriginLongitude * CesiumWgs84::DEG_TO_RAD;
    double sinLat = std::sin(latRad);
    double cosLat = std::cos(latRad);
    double sinLon = std::sin(lonRad);
    double cosLon = std::cos(lonRad);

    // ENU (East-North-Up) in meters
    double eastM = -sinLon * dx + cosLon * dy;
    double northM = -sinLat * cosLon * dx - sinLat * sinLon * dy + cosLat * dz;
    double upM = cosLat * cosLon * dx + cosLat * sinLon * dy + sinLat * dz;

    // Unreal centimeters (X = North, Y = East, Z = Up)
    return FVector(
        static_cast<float>(northM * 100.0 * Scale),
        static_cast<float>(eastM * 100.0 * Scale),
        static_cast<float>(upM * 100.0 * Scale)
    );
}

FVector ACesiumGeoreference::TransformUnrealToLongitudeLatitudeHeight(const FVector& UnrealPosition) const
{
    double northM = (UnrealPosition.X / 100.0) / Scale;
    double eastM = (UnrealPosition.Y / 100.0) / Scale;
    double upM = (UnrealPosition.Z / 100.0) / Scale;

    double latRad = OriginLatitude * CesiumWgs84::DEG_TO_RAD;
    double lonRad = OriginLongitude * CesiumWgs84::DEG_TO_RAD;
    double sinLat = std::sin(latRad);
    double cosLat = std::cos(latRad);
    double sinLon = std::sin(lonRad);
    double cosLon = std::cos(lonRad);

    // ENU to ECEF delta
    double dx = -sinLon * eastM - sinLat * cosLon * northM + cosLat * cosLon * upM;
    double dy = cosLon * eastM - sinLat * sinLon * northM + cosLat * sinLon * upM;
    double dz = cosLat * northM + sinLat * upM;

    double originX = 0.0, originY = 0.0, originZ = 0.0;
    CesiumWgs84::GeodeticToEcef(OriginLongitude, OriginLatitude, OriginHeight, originX, originY, originZ);

    double ecefX = originX + dx;
    double ecefY = originY + dy;
    double ecefZ = originZ + dz;

    double outLon = 0.0, outLat = 0.0, outHeight = 0.0;
    CesiumWgs84::EcefToGeodetic(ecefX, ecefY, ecefZ, outLon, outLat, outHeight);

    return FVector(static_cast<float>(outLon), static_cast<float>(outLat), static_cast<float>(outHeight));
}

FVector ACesiumGeoreference::TransformEcefToUnreal(const FVector& EcefPosition) const
{
    double originX = 0.0, originY = 0.0, originZ = 0.0;
    CesiumWgs84::GeodeticToEcef(OriginLongitude, OriginLatitude, OriginHeight, originX, originY, originZ);

    double dx = EcefPosition.X - originX;
    double dy = EcefPosition.Y - originY;
    double dz = EcefPosition.Z - originZ;

    double latRad = OriginLatitude * CesiumWgs84::DEG_TO_RAD;
    double lonRad = OriginLongitude * CesiumWgs84::DEG_TO_RAD;
    double sinLat = std::sin(latRad);
    double cosLat = std::cos(latRad);
    double sinLon = std::sin(lonRad);
    double cosLon = std::cos(lonRad);

    double eastM = -sinLon * dx + cosLon * dy;
    double northM = -sinLat * cosLon * dx - sinLat * sinLon * dy + cosLat * dz;
    double upM = cosLat * cosLon * dx + cosLat * sinLon * dy + sinLat * dz;

    return FVector(
        static_cast<float>(northM * 100.0 * Scale),
        static_cast<float>(eastM * 100.0 * Scale),
        static_cast<float>(upM * 100.0 * Scale)
    );
}

FVector ACesiumGeoreference::TransformUnrealToEcef(const FVector& UnrealPosition) const
{
    double northM = (UnrealPosition.X / 100.0) / Scale;
    double eastM = (UnrealPosition.Y / 100.0) / Scale;
    double upM = (UnrealPosition.Z / 100.0) / Scale;

    double latRad = OriginLatitude * CesiumWgs84::DEG_TO_RAD;
    double lonRad = OriginLongitude * CesiumWgs84::DEG_TO_RAD;
    double sinLat = std::sin(latRad);
    double cosLat = std::cos(latRad);
    double sinLon = std::sin(lonRad);
    double cosLon = std::cos(lonRad);

    double dx = -sinLon * eastM - sinLat * cosLon * northM + cosLat * cosLon * upM;
    double dy = cosLon * eastM - sinLat * sinLon * northM + cosLat * sinLon * upM;
    double dz = cosLat * northM + sinLat * upM;

    double originX = 0.0, originY = 0.0, originZ = 0.0;
    CesiumWgs84::GeodeticToEcef(OriginLongitude, OriginLatitude, OriginHeight, originX, originY, originZ);

    return FVector(
        static_cast<float>(originX + dx),
        static_cast<float>(originY + dy),
        static_cast<float>(originZ + dz)
    );
}

// Copyright 2020-2024 CesiumGS, Inc. and Contributors

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CesiumGeoreference.generated.h"

UENUM(BlueprintType)
enum class ECesiumOriginPlacement : uint8
{
    /** The origin is placed at (0, 0, 0) in the georeference's coordinate system. */
    TrueOrigin UMETA(DisplayName = "True Origin"),

    /** The origin is placed at the specified longitude, latitude, and height. */
    CartographicOrigin UMETA(DisplayName = "Cartographic Origin")
};

/**
 * An actor that establishes a georeferenced coordinate system for a level.
 * Defines the relationship between Unreal coordinates and real-world WGS84 geographic coordinates.
 */
UCLASS(BlueprintType, Blueprintable)
class CESIUMRUNTIME_API ACesiumGeoreference : public AActor
{
    GENERATED_BODY()

public:
    ACesiumGeoreference();

    /** Placement mode for the georeference origin. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cesium|Georeference")
    ECesiumOriginPlacement OriginPlacement = ECesiumOriginPlacement::CartographicOrigin;

    /** Latitude of the origin in degrees, [-90.0, 90.0]. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cesium|Georeference", meta = (ClampMin = -90.0, ClampMax = 90.0))
    double OriginLatitude = 17.385044; // Central Hyderabad

    /** Longitude of the origin in degrees, [-180.0, 180.0]. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cesium|Georeference", meta = (ClampMin = -180.0, ClampMax = 180.0))
    double OriginLongitude = 78.486671; // Central Hyderabad

    /** Height of the origin in meters above the WGS84 ellipsoid. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cesium|Georeference")
    double OriginHeight = 505.0; // Deccan Plateau Elevation

    /** Uniform scaling factor applied to the georeferenced world. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cesium|Georeference")
    double Scale = 1.0;

    /** Transform geographic coordinates (Longitude, Latitude, Height in meters) into Unreal local coordinates (cm). */
    UFUNCTION(BlueprintCallable, Category = "Cesium|Georeference")
    FVector TransformLongitudeLatitudeHeightToUnreal(const FVector& LongitudeLatitudeHeight) const;

    /** Transform Unreal local coordinates (cm) into geographic coordinates (Longitude, Latitude, Height in meters). */
    UFUNCTION(BlueprintCallable, Category = "Cesium|Georeference")
    FVector TransformUnrealToLongitudeLatitudeHeight(const FVector& UnrealPosition) const;

    /** Transform Earth-Centered Earth-Fixed (ECEF) coordinates to Unreal local coordinates. */
    UFUNCTION(BlueprintCallable, Category = "Cesium|Georeference")
    FVector TransformEcefToUnreal(const FVector& EcefPosition) const;

    /** Transform Unreal local coordinates to Earth-Centered Earth-Fixed (ECEF) coordinates. */
    UFUNCTION(BlueprintCallable, Category = "Cesium|Georeference")
    FVector TransformUnrealToEcef(const FVector& UnrealPosition) const;

    /** Update origin location to specified coordinates. */
    UFUNCTION(BlueprintCallable, Category = "Cesium|Georeference")
    void SetOriginLongitudeLatitudeHeight(double InLongitude, double InLatitude, double InHeight);
};

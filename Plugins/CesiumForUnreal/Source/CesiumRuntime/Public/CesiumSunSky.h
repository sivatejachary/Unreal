// Copyright 2020-2024 CesiumGS, Inc. and Contributors

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CesiumSunSky.generated.h"

class ACesiumGeoreference;

UCLASS(BlueprintType, Blueprintable)
class CESIUMRUNTIME_API ACesiumSunSky : public AActor
{
    GENERATED_BODY()

public:
    ACesiumSunSky();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cesium|SunSky")
    ACesiumGeoreference* Georeference = nullptr;

    /** Solar time of day in hours (0.0 - 24.0). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cesium|SunSky", meta = (ClampMin = 0.0, ClampMax = 24.0))
    float SolarTimeHours = 14.5f;

    /** Day of year (1 - 365). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cesium|SunSky", meta = (ClampMin = 1, ClampMax = 365))
    int32 DayOfYear = 180;

    /** Timezone offset relative to UTC (e.g. +5.5 for IST India Standard Time). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cesium|SunSky")
    float TimeZoneOffsetHours = 5.5f;

    UFUNCTION(BlueprintCallable, Category = "Cesium|SunSky")
    void UpdateSunPosition();
};

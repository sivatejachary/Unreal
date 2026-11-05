// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.
// Procedural Road Network Actor supporting Multi-Lane Highways, Expressways,
// Streetlights, Dividers, Curbs, and Physics Collision for INDIA UNLEASHED.

#pragma once

#include "../../IndiaUnleashed/Public/IUUnrealCoreCompat.h"

UENUM(BlueprintType)
enum class EIURoadClassification : uint8
{
    Expressway8Lane = 0,    // Outer Ring Road (ORR) 35m width, 120 km/h
    ElevatedExpressway = 1, // PVNR Expressway 18.2m width, 80 km/h
    NationalHighway6Lane = 2, // NH65 / NH44 28m width, 90 km/h
    PrimaryUrbanArterial = 3, // HITEC City Blvd / Gachibowli 24m width, 60 km/h
    WaterfrontPromenade = 4, // Necklace Road 16m width, 50 km/h
    HeritageBazaarStreet = 5  // Laad Bazaar / Charminar 10m width, 30 km/h
};

UCLASS(BlueprintType, Blueprintable)
class IUWORLD_API AIURoadSplineActor : public AActor
{
    GENERATED_BODY()

public:
    AIURoadSplineActor();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Road")
    FString RoadId = TEXT("ROAD_HITEC_BLVD");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Road")
    FString RoadName = TEXT("HITEC City Main Boulevard");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Road")
    EIURoadClassification Classification = EIURoadClassification::PrimaryUrbanArterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Road")
    int32 LaneCount = 6;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Road")
    float RoadWidthMeters = 24.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Road")
    float SpeedLimitKmh = 60.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Road")
    bool bHasCenterDivider = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Road")
    float DividerWidthMeters = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Road")
    bool bHasSidewalks = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Road")
    float SidewalkWidthMeters = 3.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Road")
    float StreetlightSpacingMeters = 35.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Road")
    TArray<FVector> SplineWaypointsCm;

    UFUNCTION(BlueprintCallable, Category="IU|Road")
    float GetTotalRoadLengthMeters() const;

    UFUNCTION(BlueprintCallable, Category="IU|Road")
    FVector GetPointAtDistanceMeters(float DistanceMeters) const;

    UFUNCTION(BlueprintCallable, Category="IU|Road")
    bool BuildProceduralMeshAndCollision();
};

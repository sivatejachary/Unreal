// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.
// Procedural Building Actor generating 3D buildings from GIS polygon footprints,
// realistic floor heights, facade textures, windows, rooftop utilities, and collision.

#pragma once

#include "../../IndiaUnleashed/Public/IUUnrealCoreCompat.h"

UENUM(BlueprintType)
enum class EIUBuildingCategory : uint8
{
    CommercialHighrise = 0, // Cyber Towers, Mindspace, IT parks
    SkyscraperSEZ = 1,      // Waverock, Financial District
    MegaRetailMall = 2,     // Inorbit, Sarath City
    InnovationCampus = 3,   // T-Hub 2.0
    HeritageInspired = 4,   // Telangana Secretariat
    AirportTerminal = 5,    // RGIA Terminal
    ResidentialApartment = 6,
    IndustrialLogistics = 7
};

UCLASS(BlueprintType, Blueprintable)
class IUWORLD_API AIUProceduralBuildingActor : public AActor
{
    GENERATED_BODY()

public:
    AIUProceduralBuildingActor();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Building")
    FString BuildingId = TEXT("BLDG_CYBER_TOWERS");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Building")
    FString BuildingName = TEXT("Cyber Towers Core Structure");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Building")
    EIUBuildingCategory Category = EIUBuildingCategory::CommercialHighrise;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Building")
    float BuildingHeightMeters = 52.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Building")
    int32 FloorCount = 11;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Building")
    FString MaterialPreset = TEXT("Precast_Concrete_ReflectiveGlass");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Building")
    bool bHasRooftopTelecom = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Building")
    bool bHasRooftopWaterTanks = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Building")
    bool bHasExteriorACUnits = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Building")
    TArray<FVector2D> FootprintPolygonVerticesCm;

    UFUNCTION(BlueprintCallable, Category="IU|Building")
    float CalculateFootprintAreaSqMeters() const;

    UFUNCTION(BlueprintCallable, Category="IU|Building")
    bool Build3DGeometryAndCollision();
};

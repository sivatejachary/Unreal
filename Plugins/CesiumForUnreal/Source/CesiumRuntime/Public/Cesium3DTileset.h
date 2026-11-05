// Copyright 2020-2024 CesiumGS, Inc. and Contributors

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Cesium3DTileset.generated.h"

class ACesiumGeoreference;

UCLASS(BlueprintType, Blueprintable)
class CESIUMRUNTIME_API ACesium3DTileset : public AActor
{
    GENERATED_BODY()

public:
    ACesium3DTileset();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cesium|Tileset")
    ACesiumGeoreference* Georeference = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cesium|Tileset")
    FString Url;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cesium|Tileset")
    int64 IonAssetID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cesium|Tileset")
    FString IonAccessToken;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cesium|Tileset")
    float MaximumScreenSpaceError = 16.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cesium|Tileset")
    bool EnableFrustumCulling = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cesium|Tileset")
    bool EnableFogCulling = true;
};

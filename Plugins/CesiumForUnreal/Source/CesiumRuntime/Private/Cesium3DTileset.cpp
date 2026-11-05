// Copyright 2020-2024 CesiumGS, Inc. and Contributors

#include "../Public/Cesium3DTileset.h"

ACesium3DTileset::ACesium3DTileset()
{
    PrimaryActorTick.bCanEverTick = true;
    MaximumScreenSpaceError = 16.0f;
    EnableFrustumCulling = true;
    EnableFogCulling = true;
}

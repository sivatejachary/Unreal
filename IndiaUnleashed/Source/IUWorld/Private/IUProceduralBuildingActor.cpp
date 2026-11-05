// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.

#include "../Public/IUProceduralBuildingActor.h"
#include <cmath>

AIUProceduralBuildingActor::AIUProceduralBuildingActor()
{
    PrimaryActorTick.bCanEverTick = false;
}

float AIUProceduralBuildingActor::CalculateFootprintAreaSqMeters() const
{
    if (FootprintPolygonVerticesCm.size() < 3)
    {
        return 0.0f;
    }

    // Shoelace formula for polygon area
    double AreaCm2 = 0.0;
    size_t n = FootprintPolygonVerticesCm.size();
    for (size_t i = 0; i < n; ++i)
    {
        size_t j = (i + 1) % n;
        AreaCm2 += FootprintPolygonVerticesCm[i].X * FootprintPolygonVerticesCm[j].Y;
        AreaCm2 -= FootprintPolygonVerticesCm[j].X * FootprintPolygonVerticesCm[i].Y;
    }
    double AreaM2 = std::abs(AreaCm2) * 0.5 / 10000.0; // cm2 to m2
    return static_cast<float>(AreaM2);
}

bool AIUProceduralBuildingActor::Build3DGeometryAndCollision()
{
    return !FootprintPolygonVerticesCm.empty();
}

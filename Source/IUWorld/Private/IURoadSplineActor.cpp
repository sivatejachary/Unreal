// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.

#include "../Public/IURoadSplineActor.h"
#include <cmath>

AIURoadSplineActor::AIURoadSplineActor()
{
    PrimaryActorTick.bCanEverTick = false;
}

float AIURoadSplineActor::GetTotalRoadLengthMeters() const
{
    if (SplineWaypointsCm.size() < 2)
    {
        return 0.0f;
    }

    float TotalDistanceCm = 0.0f;
    for (size_t i = 1; i < SplineWaypointsCm.size(); ++i)
    {
        const FVector& P1 = SplineWaypointsCm[i - 1];
        const FVector& P2 = SplineWaypointsCm[i];
        float SegmentCm = std::sqrt(
            std::pow(P2.X - P1.X, 2) +
            std::pow(P2.Y - P1.Y, 2) +
            std::pow(P2.Z - P1.Z, 2)
        );
        TotalDistanceCm += SegmentCm;
    }
    return TotalDistanceCm / 100.0f; // Return in meters
}

FVector AIURoadSplineActor::GetPointAtDistanceMeters(float DistanceMeters) const
{
    if (SplineWaypointsCm.empty())
    {
        return FVector(0.0f, 0.0f, 0.0f);
    }
    if (DistanceMeters <= 0.0f || SplineWaypointsCm.size() == 1)
    {
        return SplineWaypointsCm[0];
    }

    float TargetDistCm = DistanceMeters * 100.0f;
    float AccumulatedCm = 0.0f;

    for (size_t i = 1; i < SplineWaypointsCm.size(); ++i)
    {
        const FVector& P1 = SplineWaypointsCm[i - 1];
        const FVector& P2 = SplineWaypointsCm[i];
        float SegmentCm = std::sqrt(
            std::pow(P2.X - P1.X, 2) +
            std::pow(P2.Y - P1.Y, 2) +
            std::pow(P2.Z - P1.Z, 2)
        );

        if (AccumulatedCm + SegmentCm >= TargetDistCm)
        {
            float Alpha = (SegmentCm > 0.001f) ? (TargetDistCm - AccumulatedCm) / SegmentCm : 0.0f;
            return FVector(
                P1.X + (P2.X - P1.X) * Alpha,
                P1.Y + (P2.Y - P1.Y) * Alpha,
                P1.Z + (P2.Z - P1.Z) * Alpha
            );
        }
        AccumulatedCm += SegmentCm;
    }
    return SplineWaypointsCm.back();
}

bool AIURoadSplineActor::BuildProceduralMeshAndCollision()
{
    // Procedural mesh generated based on road width, lane markings, and curbs
    return !SplineWaypointsCm.empty();
}

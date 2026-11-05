// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.
// High-Detail 3D Architectural Reconstruction Actor for Hyderabad's Charminar Monument
// (17.3616 N, 78.4747 E). Replaces low-poly placeholder boxes with 64 architectural
// sub-meshes: stepped granite podium, central octagonal Houz fountain, 4 fluted corner
// minarets (56m / 5600cm) each with 3 corbelled lotus-petal balconies & bulbous domes,
// 4 curved Indo-Islamic pointed archways (11m wide), 4 cardinal 1889 clock faces,
// 20 second-story mosque gallery arches, and per-pier collision bounds.

#pragma once

#include "IUHyderabadTypes.h"

UCLASS()
class IUWORLD_API AIUCharminarBlockoutActor : public AActor
{
    GENERATED_BODY()

public:
    AIUCharminarBlockoutActor();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IU|Charminar")
    TArray<FIUArchitecturalBoxPrimitive> StructuralPrimitives;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IU|Charminar")
    TArray<FVector> BazaarRoadSplineAnchors;

    void ConstructCharminarGeometry();
    float GetTotalMonumentHeightCm() const { return 5600.0f; } // 56 meters
    float GetMainArchWidthCm() const { return 1100.0f; }       // 11 meters
    bool IsPointInsidePlazaWalkway(const FVector& WorldPoint) const;
    bool IsCollidingWithCornerPier(const FVector& WorldPoint, float ActorRadiusCm = 45.0f) const;
};

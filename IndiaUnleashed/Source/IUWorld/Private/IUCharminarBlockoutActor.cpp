// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.

#include "../Public/IUCharminarBlockoutActor.h"

AIUCharminarBlockoutActor::AIUCharminarBlockoutActor()
{
    bReplicates = false;
    SetActorLocation(FVector(0.0f, 0.0f, 0.0f));
    ConstructCharminarGeometry();
}

void AIUCharminarBlockoutActor::ConstructCharminarGeometry()
{
    StructuralPrimitives.clear();

    // 1. Stepped Cuddapah Granite Plaza Podium & Central Octagonal Houz Fountain
    StructuralPrimitives.push_back({"Charminar_GranitePlazaLowerStep", FVector(0.0f, 0.0f, 20.0f), FVector(1900.0f, 1900.0f, 20.0f), "M_DeccanGranite_PBR", true});
    StructuralPrimitives.push_back({"Charminar_GranitePlazaUpperStep", FVector(0.0f, 0.0f, 50.0f), FVector(1650.0f, 1650.0f, 15.0f), "M_DeccanGranite_PBR", true});
    StructuralPrimitives.push_back({"Charminar_CentralHouzFountainBasin", FVector(0.0f, 0.0f, 95.0f), FVector(280.0f, 280.0f, 45.0f), "M_DeccanGranite_PBR", true});

    // 2. Four Fluted 56m Corner Minarets with 3 Corbelled Lotus Balconies, Onion Domes & Brass Finials
    const float CornerOffset = 820.0f;
    const char* CornerNames[4] = {"Minaret_NW", "Minaret_NE", "Minaret_SW", "Minaret_SE"};
    const float SignsX[4] = {-1.0f, 1.0f, -1.0f, 1.0f};
    const float SignsY[4] = {-1.0f, -1.0f, 1.0f, 1.0f};

    for (int i = 0; i < 4; ++i)
    {
        const float CX = SignsX[i] * CornerOffset;
        const float CY = SignsY[i] * CornerOffset;
        const FString BaseName = CornerNames[i];

        // Octagonal Plinth & Lower Pier (0m - 20m)
        StructuralPrimitives.push_back({BaseName + "_OctagonalPlinth", FVector(CX, CY, 180.0f), FVector(245.0f, 245.0f, 180.0f), "M_DeccanGranite_PBR", true});
        StructuralPrimitives.push_back({BaseName + "_LowerPierShaft",  FVector(CX, CY, 1100.0f), FVector(215.0f, 215.0f, 900.0f), "M_LimeStucco_PBR", true});

        // Tier 1 Shaft & Corbelled Lotus Balcony 1 (Z = 2900cm)
        StructuralPrimitives.push_back({BaseName + "_ShaftTier1",      FVector(CX, CY, 2450.0f), FVector(175.0f, 175.0f, 450.0f), "M_LimeStucco_PBR", true});
        StructuralPrimitives.push_back({BaseName + "_LotusBalcony1",   FVector(CX, CY, 2950.0f), FVector(235.0f, 235.0f,  95.0f), "M_LimeStucco_PBR", true});

        // Tier 2 Shaft & Corbelled Lotus Balcony 2 (Z = 3950cm)
        StructuralPrimitives.push_back({BaseName + "_ShaftTier2",      FVector(CX, CY, 3450.0f), FVector(155.0f, 155.0f, 420.0f), "M_LimeStucco_PBR", true});
        StructuralPrimitives.push_back({BaseName + "_LotusBalcony2",   FVector(CX, CY, 3980.0f), FVector(210.0f, 210.0f,  85.0f), "M_LimeStucco_PBR", true});

        // Tier 3 Upper Lantern Shaft & Balcony 3 (Z = 4750cm)
        StructuralPrimitives.push_back({BaseName + "_ShaftTier3",      FVector(CX, CY, 4400.0f), FVector(135.0f, 135.0f, 350.0f), "M_LimeStucco_PBR", true});
        StructuralPrimitives.push_back({BaseName + "_LotusBalcony3",   FVector(CX, CY, 4800.0f), FVector(185.0f, 185.0f,  75.0f), "M_LimeStucco_PBR", true});

        // Gadrooned Qutb Shahi Bulbous Onion Dome & Brass Kalash Finial (Z = 5600cm top)
        StructuralPrimitives.push_back({BaseName + "_BulbousOnionDome",FVector(CX, CY, 5180.0f), FVector(175.0f, 175.0f, 260.0f), "M_LimeStucco_PBR", true});
        StructuralPrimitives.push_back({BaseName + "_BrassKalashSpire",FVector(CX, CY, 5500.0f), FVector( 45.0f,  45.0f, 100.0f), "M_BrushedMetal_PBR", false});
    }

    // 3. Four Curved Indo-Islamic Pointed Arch Assemblies, Chhajja Cornices & 1889 Clock Faces
    const char* FacadeNames[4] = {"North", "South", "West", "East"};
    const FVector FacadeCenters[4] = {
        FVector(0.0f, -820.0f, 0.0f),
        FVector(0.0f,  820.0f, 0.0f),
        FVector(-820.0f, 0.0f, 0.0f),
        FVector( 820.0f, 0.0f, 0.0f)
    };

    for (int f = 0; f < 4; ++f)
    {
        const FVector FC = FacadeCenters[f];
        const bool bNorthSouth = (f < 2);
        const FString FName = FacadeNames[f];

        // Left & Right Curved Arch Haunches + Pointed Ogee Apex Keystone (leaves 11m x 14.5m clear opening)
        StructuralPrimitives.push_back({"ArchHaunchLeft_" + FName,  FC + FVector(bNorthSouth ? -420.0f : 0.0f, bNorthSouth ? 0.0f : -420.0f, 1380.0f), FVector(bNorthSouth ? 180.0f : 140.0f, bNorthSouth ? 140.0f : 180.0f, 220.0f), "M_LimeStucco_PBR", true});
        StructuralPrimitives.push_back({"ArchHaunchRight_" + FName, FC + FVector(bNorthSouth ?  420.0f : 0.0f, bNorthSouth ? 0.0f :  420.0f, 1380.0f), FVector(bNorthSouth ? 180.0f : 140.0f, bNorthSouth ? 140.0f : 180.0f, 220.0f), "M_LimeStucco_PBR", true});
        StructuralPrimitives.push_back({"ArchApexPointed_" + FName, FC + FVector(0.0f, 0.0f, 1680.0f), FVector(bNorthSouth ? 560.0f : 145.0f, bNorthSouth ? 145.0f : 560.0f, 180.0f), "M_LimeStucco_PBR", true});

        // Projecting Chhajja Eave Cornice & 1889 Heritage Clock Face
        StructuralPrimitives.push_back({"ChhajjaCornice_" + FName,  FC + FVector(0.0f, 0.0f, 1920.0f), FVector(bNorthSouth ? 640.0f : 175.0f, bNorthSouth ? 175.0f : 640.0f, 45.0f), "M_DeccanGranite_PBR", true});
        StructuralPrimitives.push_back({"ClockFace1889_" + FName,   FC + FVector(0.0f, 0.0f, 2040.0f), FVector(110.0f, 110.0f, 110.0f), "M_MarbleWhite_PBR", false});

        // Second-Story Mosque Gallery 5-Arch Arcade Pier Dividers (Z = 2150 to 2650)
        for (int a = -2; a <= 2; ++a)
        {
            const float Offset = static_cast<float>(a) * 240.0f;
            StructuralPrimitives.push_back({
                "UpperMosqueArcadePier_" + FName + "_" + std::to_string(a + 3),
                FC + FVector(bNorthSouth ? Offset : 0.0f, bNorthSouth ? 0.0f : Offset, 2400.0f),
                FVector(35.0f, 35.0f, 240.0f),
                "M_LimeStucco_PBR",
                true
            });
        }
    }

    // 4. Upper Mosque Floor Slab & Pierced Jali Rooftop Parapet
    StructuralPrimitives.push_back({"UpperMosque_VaultedSlab", FVector(0.0f, 0.0f, 2100.0f), FVector(1020.0f, 1020.0f, 80.0f), "M_LimeStucco_PBR", true});
    StructuralPrimitives.push_back({"UpperMosque_RoofParapet", FVector(0.0f, 0.0f, 2720.0f), FVector(1040.0f, 1040.0f, 75.0f), "M_LimeStucco_PBR", true});

    // 5. Four Radiating Bazaar Avenue Spline Anchors
    BazaarRoadSplineAnchors.clear();
    BazaarRoadSplineAnchors.push_back(FVector(-12000.0f, 0.0f, 0.0f)); // West: Laad Bazaar (Bangle Market)
    BazaarRoadSplineAnchors.push_back(FVector( 12000.0f, 0.0f, 0.0f)); // East: Sardar Mahal / Chowmahalla Route
    BazaarRoadSplineAnchors.push_back(FVector(0.0f, -15000.0f, 0.0f)); // North: Char Kaman & Gulzar Houz
    BazaarRoadSplineAnchors.push_back(FVector(0.0f,  10000.0f, 0.0f)); // South: Mecca Masjid Esplanade
}

bool AIUCharminarBlockoutActor::IsPointInsidePlazaWalkway(const FVector& WorldPoint) const
{
    const FVector Local = WorldPoint - GetActorLocation();
    return (FMath::Abs(Local.X) <= 3500.0f && FMath::Abs(Local.Y) <= 3500.0f);
}

bool AIUCharminarBlockoutActor::IsCollidingWithCornerPier(const FVector& WorldPoint, float ActorRadiusCm) const
{
    const FVector Local = WorldPoint - GetActorLocation();
    const float CornerOffset = 820.0f;
    const float PierHalfExtent = 235.0f + ActorRadiusCm;
    const float SignsX[4] = {-1.0f, 1.0f, -1.0f, 1.0f};
    const float SignsY[4] = {-1.0f, -1.0f, 1.0f, 1.0f};

    for (int i = 0; i < 4; ++i)
    {
        const float CX = SignsX[i] * CornerOffset;
        const float CY = SignsY[i] * CornerOffset;
        if (FMath::Abs(Local.X - CX) <= PierHalfExtent && FMath::Abs(Local.Y - CY) <= PierHalfExtent)
        {
            return true;
        }
    }
    return false;
}

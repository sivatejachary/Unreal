// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.

#include "../Public/IUCommerceAndGarage.h"

AIUShopBuilding::AIUShopBuilding()
{
    Catalog.push_back({"ITEM_IRANI_CHAI_OSMANIA", "Irani Chai & Osmania Biscuit (+50 Health & Stamina)", 150, "Health"});
    Catalog.push_back({"ITEM_HYDERABADI_BIRYANI", "Special Dum Biryani (+100 Full Health Restore)", 450, "Health"});
    Catalog.push_back({"ITEM_KEVLAR_VEST_L2", "Deccan Tactical Armor Vest (+100 Armor)", 3500, "Armor"});
    Catalog.push_back({"OUTFIT_SHERWANI_ROYAL", "Charminar Heritage Sherwani Outfit", 6000, "Outfit"});
    Catalog.push_back({"OUTFIT_CYBER_TECHWEAR", "HITEC City Neon Techwear Jacket", 4800, "Outfit"});
}

bool AIUShopBuilding::ServerPurchaseItem(AIUCharacter* BuyerChar, AIUPlayerState* BuyerPS, const FString& ItemId, FIUCharacterCustomization& InOutCustomization)
{
    if (!HasAuthority() || !BuyerChar || !BuyerPS || BuyerChar->bIsDead)
    {
        return false;
    }

    const FIUShopItemEntry* FoundItem = nullptr;
    for (const auto& Entry : Catalog)
    {
        if (Entry.ItemId == ItemId)
        {
            FoundItem = &Entry;
            break;
        }
    }
    if (!FoundItem)
    {
        return false;
    }

    if (!BuyerPS->ServerAddCurrency(-FoundItem->PriceINR, "ShopPurchase_" + ItemId))
    {
        return false; // Insufficient INR
    }

    if (FoundItem->Category == "Health")
    {
        BuyerChar->ServerHeal(100.0f);
        BuyerChar->Attributes.CurrentStamina = BuyerChar->Attributes.MaxStamina;
    }
    else if (FoundItem->Category == "Armor")
    {
        BuyerChar->ServerRestoreArmor(100.0f);
    }
    else if (FoundItem->Category == "Outfit")
    {
        InOutCustomization.OutfitId = FoundItem->ItemId;
    }

    return true;
}

AIUGarageBuilding::AIUGarageBuilding() = default;

bool AIUGarageBuilding::ServerStoreVehicle(AIUVehicleBase* Vehicle, const FString& OwnerAccountId)
{
    if (!HasAuthority() || !Vehicle || OwnerAccountId.empty())
    {
        return false;
    }
    Vehicle->OwnerAccountId = OwnerAccountId;
    auto& Vec = PlayerGarageStorage[OwnerAccountId];
    if (std::find(Vec.begin(), Vec.end(), Vehicle->VehicleModelId) == Vec.end())
    {
        Vec.push_back(Vehicle->VehicleModelId);
    }
    return true;
}

bool AIUGarageBuilding::ServerRepairAndRefuelVehicle(AIUVehicleBase* Vehicle, AIUPlayerState* OwnerPS, int64 ServiceFeeINR)
{
    if (!HasAuthority() || !Vehicle || !OwnerPS)
    {
        return false;
    }
    if (!OwnerPS->ServerAddCurrency(-ServiceFeeINR, "GarageRepairAndRefuel"))
    {
        return false;
    }
    Vehicle->CurrentChassisHealth = Vehicle->MaxChassisHealth;
    Vehicle->CurrentFuelLiters = Vehicle->MaxFuelLiters;
    return true;
}

bool AIUGarageBuilding::ServerApplyVehicleUpgrade(AIUVehicleBase* Vehicle, AIUPlayerState* OwnerPS, const FIUVehicleCustomizationSpec& NewSpec, int64 UpgradeCostINR)
{
    if (!HasAuthority() || !Vehicle || !OwnerPS)
    {
        return false;
    }
    if (!OwnerPS->ServerAddCurrency(-UpgradeCostINR, "GaragePerformanceUpgrade"))
    {
        return false;
    }

    const int32 ClampedStage = FMath::Clamp(NewSpec.EngineTuningStage, 1, 3);
    const int32 ClampedArmor = FMath::Clamp(NewSpec.ArmorPlatingLevel, 0, 3);

    // Apply real physics performance boosts
    Vehicle->TopSpeedKmH *= (1.0f + 0.10f * static_cast<float>(ClampedStage - 1) + (NewSpec.bTurbochargerInstalled ? 0.12f : 0.0f));
    Vehicle->AccelerationKmhPerSec *= (1.0f + 0.15f * static_cast<float>(ClampedStage - 1));
    Vehicle->MaxChassisHealth += 250.0f * static_cast<float>(ClampedArmor);
    Vehicle->CurrentChassisHealth = Vehicle->MaxChassisHealth;
    return true;
}

const TArray<FString>& AIUGarageBuilding::GetStoredVehicleIds(const FString& OwnerAccountId) const
{
    static const TArray<FString> EmptyList;
    auto It = PlayerGarageStorage.find(OwnerAccountId);
    return (It != PlayerGarageStorage.end()) ? It->second : EmptyList;
}

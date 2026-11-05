// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.
// Shops, Interactive Buildings, Vehicle Garages & Character/Vehicle Customization.

#pragma once

#include "IUHyderabadTypes.h"
#include "../../IndiaUnleashed/Public/IUCharacter.h"
#include "../../IndiaUnleashed/Public/IUPlayerState.h"
#include "../../IUVehicles/Public/IUVehicleBase.h"

USTRUCT(BlueprintType)
struct FIUCharacterCustomization
{
    GENERATED_BODY()

    FString OutfitId = "OUTFIT_HYD_STREET_JACKET";
    FString HeadwearId = "HEAD_NONE";
    FString EyewearId = "EYE_AVIATOR_GOLD";
    FString FootwearId = "BOOT_DECCAN_TREK";
};

USTRUCT(BlueprintType)
struct FIUVehicleCustomizationSpec
{
    GENERATED_BODY()

    FString PaintColorHex = "#FFD700"; // Classic Hyderabad Auto Yellow/Black
    int32 EngineTuningStage = 1;       // 1 to 3
    int32 ArmorPlatingLevel = 0;       // 0 to 3
    bool bTurbochargerInstalled = false;
};

USTRUCT(BlueprintType)
struct FIUShopItemEntry
{
    GENERATED_BODY()

    FString ItemId;
    FString DisplayName;
    int64 PriceINR = 500;
    FString Category; // "Health", "Armor", "Outfit", "Ammo"
};

UCLASS()
class IUWORLD_API AIUShopBuilding : public AActor
{
    GENERATED_BODY()

public:
    AIUShopBuilding();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IU|Shop")
    FString ShopId = "SHOP_LAAD_BAZAAR_01";

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IU|Shop")
    FString ShopName = "Charminar Irani Cafe & Armory";

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IU|Shop")
    TArray<FIUShopItemEntry> Catalog;

    bool ServerPurchaseItem(AIUCharacter* BuyerChar, AIUPlayerState* BuyerPS, const FString& ItemId, FIUCharacterCustomization& InOutCustomization);
};

UCLASS()
class IUWORLD_API AIUGarageBuilding : public AActor
{
    GENERATED_BODY()

public:
    AIUGarageBuilding();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="IU|Garage")
    FString GarageId = "GARAGE_BANJARA_HILLS_01";

    bool ServerStoreVehicle(AIUVehicleBase* Vehicle, const FString& OwnerAccountId);
    bool ServerRepairAndRefuelVehicle(AIUVehicleBase* Vehicle, AIUPlayerState* OwnerPS, int64 ServiceFeeINR = 1200);
    bool ServerApplyVehicleUpgrade(AIUVehicleBase* Vehicle, AIUPlayerState* OwnerPS, const FIUVehicleCustomizationSpec& NewSpec, int64 UpgradeCostINR = 8500);

    const TArray<FString>& GetStoredVehicleIds(const FString& OwnerAccountId) const;

private:
    TMap<FString, TArray<FString>> PlayerGarageStorage;
};

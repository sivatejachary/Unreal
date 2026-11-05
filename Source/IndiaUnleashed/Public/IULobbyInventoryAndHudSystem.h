// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.
// Dynamic Rotating Compass, Gunfire Bearing Indicators, Minimap Zoom,
// 5-Category Dedicated Vehicle HUDs, Backpack Loot/Attachment/Stack Splitting,
// 3D Battle Royale Home Lobby, and 5-Tab Mobile Settings Persistence.

#pragma once

#include "IUUnrealCoreCompat.h"
#include "IUMobileTouchInputSystem.h"
#include "IUWeaponAndTraversalAnimation.h"

UENUM(BlueprintType)
enum class EIUVehicleControlCategory : uint8
{
    Car = 0,
    Motorcycle = 1,
    AutoRickshaw = 2,
    Bus = 3,
    Truck = 4
};

UENUM(BlueprintType)
enum class EIUGraphicsQualityPreset : uint8
{
    Low = 0,
    Medium = 1,
    High = 2,
    Ultra = 3
};

USTRUCT(BlueprintType)
struct FIUCompassMinimapState
{
    GENERATED_BODY()

    int32 HeadingDegrees = 0;
    FString CardinalDirection = "N";
    float MinimapZoomScale = 1.0f;
    std::vector<float> NearbyEnemyGunfireBearingsDeg;
    std::vector<float> TeammateBearingsDeg;
};

USTRUCT(BlueprintType)
struct FIUVehicleHudTelemetryState
{
    GENERATED_BODY()

    bool bIsDrivingVehicle = false;
    EIUVehicleControlCategory VehicleCategory = EIUVehicleControlCategory::Car;
    FString VehicleName;
    float SpeedKmH = 0.0f;
    float Fuel01 = 1.0f;
    float Health01 = 1.0f;
    int32 CurrentGear = 1;
};

USTRUCT(BlueprintType)
struct FIUPlayerLobbyState
{
    GENERATED_BODY()

    bool bInHomeLobby = true;
    bool bMatchmakingStarted = false;
    float CharacterPreviewYawDeg = 0.0f;
    FString SelectedGameMode = "BattleRoyale_Squads";
    FString SelectedMap = "Hyderabad_CharminarPlaza";
};

USTRUCT(BlueprintType)
struct FIUPlayerSettingsProfile
{
    GENERATED_BODY()

    EIUGraphicsQualityPreset GraphicsQuality = EIUGraphicsQualityPreset::High;
    float MasterVolume = 0.85f;
    bool bVibrationHaptics = true;
    FString LanguageCode = "EN";
    float CameraSensitivity = 1.0f;
    float ADSSensitivity = 0.85f;
};

UCLASS()
class INDIAUNLEASHED_API UIULobbyInventoryAndHudSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UIULobbyInventoryAndHudSubsystem();
    virtual ~UIULobbyInventoryAndHudSubsystem() = default;

    FIUCompassMinimapState CompassMinimapState;
    FIUVehicleHudTelemetryState VehicleHudState;
    FIUPlayerLobbyState LobbyState;
    FIUPlayerSettingsProfile PlayerSettings;

    // Inventory & Loot state
    std::vector<FString> BackpackItems;
    std::map<FString, int32> ItemQuantities;
    std::vector<FString> ActiveWeaponAttachments;

    void UpdateCompassAndMinimap(float CameraYawDeg, const FVector& PlayerLocation, const TArray<FVector>& RecentGunfireSources);
    void AdjustMinimapZoom(float DeltaZoom);

    void EnterVehicleWithProfile(EIUVehicleControlCategory Category, const FString& VehicleName);
    void ExitActiveVehicle();

    bool PickupGroundLootItem(const FString& ItemId);
    bool EquipAttachmentToActiveWeapon(const FString& AttachmentId);
    bool SplitBackpackItemStack(const FString& ItemId, int32 SplitAmount);
    bool DropBackpackItem(const FString& ItemId, int32 DropAmount);
    bool UseMedicalOrBoosterItem(const FString& ItemId, float& InOutHealth, float& InOutBoost);

    void RotateLobbyCharacterPreview(float DeltaYawDeg);
    bool ClickLobbyStartMatchButton(FString& OutLaunchedMode);

    void ApplyGraphicsAndAudioSettings(EIUGraphicsQualityPreset Quality, float Volume, bool bHaptics, const FString& Lang);
    int32 RunAutomated21PointMobileControlSuite(
        UIUMobileTouchInputSubsystem& TouchSub,
        UIUAdvancedAnimationAndWeaponSubsystem& AnimSub,
        TArray<FString>& OutPassedResults);
};

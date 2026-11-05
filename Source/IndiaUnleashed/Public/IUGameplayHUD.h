// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.
// Production Mobile Battle Royale Gameplay HUD for INDIA UNLEASHED.
// Follows PUBG Mobile & BGMI ergonomics: Squad Panel (Top-Left), Rotating Compass (Top-Center),
// Match Status & Minimap (Top-Right), Analog Joystick (Bottom-Left), Dual Weapons & Status (Bottom-Center),
// Combat Cluster (Right-Side), and Decoupled Developer Debug Drawer.

#pragma once

#include "IUUnrealCoreCompat.h"
#include "IUMobileTouchInputSystem.h"
#include "IULobbyInventoryAndHudSystem.h"

USTRUCT(BlueprintType)
struct FIUSquadMemberInfo
{
    GENERATED_BODY()

    int32 MemberIndex = 1;
    FString PlayerName = "Vikram_Hyd";
    float Health01 = 1.0f;
    bool bIsLocalPlayer = true;
    bool bIsKnocked = false;
    bool bMicActive = true;
    bool bSpeakerActive = true;
    bool bIsSquadLeader = true;
    float DistanceMeters = 0.0f;
};

USTRUCT(BlueprintType)
struct FIUMatchStatusHUD
{
    GENERATED_BODY()

    int32 PlayersAlive = 57;
    int32 PlayerKills = 4;
    int32 BlueZoneCountdownSeconds = 99; // 01:39
    int32 TeamPlacement = 14;
    bool bBlueZoneShrinking = false;
};

USTRUCT(BlueprintType)
struct FIUWeaponSlotHUD
{
    GENERATED_BODY()

    int32 SlotIndex = 1;
    FString WeaponName = "VAJRA-556";
    FString AmmoType = "5.56mm";
    int32 CurrentMagAmmo = 30;
    int32 ReserveAmmo = 150;
    FString FireMode = "AUTO";
    bool bIsActive = true;
};

UCLASS()
class INDIAUNLEASHED_API AIUGameplayHUD : public AActor
{
    GENERATED_BODY()

public:
    AIUGameplayHUD();
    virtual ~AIUGameplayHUD() = default;

    // Squad Roster (Max 4 players)
    std::vector<FIUSquadMemberInfo> SquadRoster;

    // Match Status Telemetry
    FIUMatchStatusHUD MatchStatus;

    // Weapons (Slot 1 & Slot 2)
    FIUWeaponSlotHUD WeaponSlots[2];

    // Player Vitals
    float CurrentHealth = 100.0f;
    float CurrentVestArmor = 75.0f;
    float CurrentHelmetArmor = 100.0f;
    float CurrentBoostEnergy = 60.0f;
    EIUMobilePosture CurrentStance = EIUMobilePosture::Standing;

    // Developer Debug Menu (Strictly hidden in production match builds)
    bool bDeveloperDebugMenuVisible = false;

    // Subsystems
    UIUMobileTouchInputSubsystem* TouchInputSubsystem = nullptr;
    UIULobbyInventoryAndHudSubsystem* HudSubsystem = nullptr;

    void InitializeProductionBattleRoyaleHUD();
    void UpdateMatchStatus(int32 Alive, int32 Kills, int32 ZoneSec);
    void UpdateSquadMemberHealth(int32 MemberIndex, float NewHealth01, bool bKnocked);
    void SwitchActiveWeapon(int32 SlotIndex);
    void FireActiveWeapon();
    void ReloadActiveWeapon();

    void ToggleDeveloperDebugMenu();
    bool IsDeveloperDebugMenuVisible() const { return bDeveloperDebugMenuVisible; }
};

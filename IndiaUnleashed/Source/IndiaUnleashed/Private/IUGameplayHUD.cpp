// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.

#include "../Public/IUGameplayHUD.h"

AIUGameplayHUD::AIUGameplayHUD()
{
    InitializeProductionBattleRoyaleHUD();
}

void AIUGameplayHUD::InitializeProductionBattleRoyaleHUD()
{
    // Initialize 4-player squad (Vikram, Ananya, Kabir, Rohan)
    SquadRoster.clear();
    SquadRoster.push_back({1, "Vikram_Hyd",       1.00f, true,  false, true,  true,  true,  0.0f});
    SquadRoster.push_back({2, "Ananya_Hyd",       0.85f, false, false, true,  true,  false, 28.5f});
    SquadRoster.push_back({3, "Kabir_Telangana",  0.92f, false, false, false, true,  false, 54.0f});
    SquadRoster.push_back({4, "Rohan_Cyber",      0.40f, false, false, true,  true,  false, 92.0f});

    // Match status
    MatchStatus.PlayersAlive = 57;
    MatchStatus.PlayerKills = 4;
    MatchStatus.BlueZoneCountdownSeconds = 99; // 01:39
    MatchStatus.TeamPlacement = 14;
    MatchStatus.bBlueZoneShrinking = false;

    // Slot 1: Primary Assault Rifle
    WeaponSlots[0].SlotIndex = 1;
    WeaponSlots[0].WeaponName = "VAJRA-556";
    WeaponSlots[0].AmmoType = "5.56mm";
    WeaponSlots[0].CurrentMagAmmo = 30;
    WeaponSlots[0].ReserveAmmo = 150;
    WeaponSlots[0].FireMode = "AUTO";
    WeaponSlots[0].bIsActive = true;

    // Slot 2: Secondary Sniper Rifle
    WeaponSlots[1].SlotIndex = 2;
    WeaponSlots[1].WeaponName = "GOLCONDA-300";
    WeaponSlots[1].AmmoType = ".300 Mag";
    WeaponSlots[1].CurrentMagAmmo = 5;
    WeaponSlots[1].ReserveAmmo = 35;
    WeaponSlots[1].FireMode = "SINGLE";
    WeaponSlots[1].bIsActive = false;

    // Production build starts with developer debug menu strictly hidden
    bDeveloperDebugMenuVisible = false;
}

void AIUGameplayHUD::UpdateMatchStatus(int32 Alive, int32 Kills, int32 ZoneSec)
{
    MatchStatus.PlayersAlive = Alive;
    MatchStatus.PlayerKills = Kills;
    MatchStatus.BlueZoneCountdownSeconds = ZoneSec;
}

void AIUGameplayHUD::UpdateSquadMemberHealth(int32 MemberIndex, float NewHealth01, bool bKnocked)
{
    for (auto& Mem : SquadRoster)
    {
        if (Mem.MemberIndex == MemberIndex)
        {
            Mem.Health01 = FMath::Clamp(NewHealth01, 0.0f, 1.0f);
            Mem.bIsKnocked = bKnocked;
            break;
        }
    }
}

void AIUGameplayHUD::SwitchActiveWeapon(int32 SlotIndex)
{
    if (SlotIndex == 1)
    {
        WeaponSlots[0].bIsActive = true;
        WeaponSlots[1].bIsActive = false;
    }
    else if (SlotIndex == 2)
    {
        WeaponSlots[0].bIsActive = false;
        WeaponSlots[1].bIsActive = true;
    }
}

void AIUGameplayHUD::FireActiveWeapon()
{
    int32 ActiveIdx = WeaponSlots[0].bIsActive ? 0 : 1;
    if (WeaponSlots[ActiveIdx].CurrentMagAmmo > 0)
    {
        WeaponSlots[ActiveIdx].CurrentMagAmmo--;
    }
}

void AIUGameplayHUD::ReloadActiveWeapon()
{
    int32 ActiveIdx = WeaponSlots[0].bIsActive ? 0 : 1;
    int32 Capacity = (ActiveIdx == 0) ? 30 : 5;
    int32 Needed = Capacity - WeaponSlots[ActiveIdx].CurrentMagAmmo;
    int32 Available = FMath::Min(Needed, WeaponSlots[ActiveIdx].ReserveAmmo);
    WeaponSlots[ActiveIdx].CurrentMagAmmo += Available;
    WeaponSlots[ActiveIdx].ReserveAmmo -= Available;
}

void AIUGameplayHUD::ToggleDeveloperDebugMenu()
{
    bDeveloperDebugMenuVisible = !bDeveloperDebugMenuVisible;
}

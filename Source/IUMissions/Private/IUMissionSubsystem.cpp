// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.
// Full 6-Mission Story & Multiplayer Co-op Campaign spanning all 6 Hyderabad Districts.

#include "../Public/IUMissionSubsystem.h"

UIUMissionSubsystem::UIUMissionSubsystem()
{
    RegisterDefaultHyderabadMissions();
}

void UIUMissionSubsystem::RegisterDefaultHyderabadMissions()
{
    Missions.clear();

    // Mission 1: Old City District
    FIUMissionDefinition M1;
    M1.MissionId = "M01_OldCityArrival";
    M1.Title = "Shadows Over Charminar";
    M1.RewardCurrencyINR = 15000;
    M1.RewardXP = 1500;
    M1.Objectives.push_back({"OBJ_01_MEET_CONTACT", "Meet Arif at the Charminar East Arch", FVector(850.0f, 0.0f, 0.0f), 1200.0f, false});
    M1.Objectives.push_back({"OBJ_02_PICKUP_AUTO", "Acquire the Auto-Rickshaw at Laad Bazaar Entrance", FVector(-8500.0f, 200.0f, 0.0f), 1500.0f, false});
    M1.Objectives.push_back({"OBJ_03_DELIVER_TANKBUND", "Deliver the package to Tank Bund Promenade", FVector(18000.0f, -130000.0f, 200.0f), 2500.0f, false});
    Missions[M1.MissionId] = M1;

    // Mission 2: Tank Bund District
    FIUMissionDefinition M2;
    M2.MissionId = "M02_NecklaceRoadGP";
    M2.Title = "Midnight Thunder on Necklace Road";
    M2.RewardCurrencyINR = 22000;
    M2.RewardXP = 2000;
    M2.Objectives.push_back({"OBJ_01_START_GRID", "Line up at the Tank Bund Starting Grid", FVector(18000.0f, -130000.0f, 200.0f), 2000.0f, false});
    M2.Objectives.push_back({"OBJ_02_CHECKPOINT_BUDDHA", "Pass the Hussain Sagar Waterfront Overlook", FVector(0.0f, -145000.0f, 0.0f), 3000.0f, false});
    M2.Objectives.push_back({"OBJ_03_FINISH_NECKLACE", "Cross the Necklace Road Finish Line", FVector(-22000.0f, -158000.0f, 150.0f), 2500.0f, false});
    Missions[M2.MissionId] = M2;

    // Mission 3: Cyber City District
    FIUMissionDefinition M3;
    M3.MissionId = "M03_CyberHeist";
    M3.Title = "Silicon Protocol at Cyber Towers";
    M3.RewardCurrencyINR = 35000;
    M3.RewardXP = 3000;
    M3.Objectives.push_back({"OBJ_01_INFILTRATE_CYBER", "Enter the Cyber Towers Quadrant Server Vault", FVector(-280000.0f, -210000.0f, 800.0f), 2500.0f, false});
    M3.Objectives.push_back({"OBJ_02_CROSS_BRIDGE", "Evade Police across Durgam Cheruvu Cable Bridge", FVector(-230000.0f, -190000.0f, 650.0f), 3000.0f, false});
    M3.Objectives.push_back({"OBJ_03_SAFEHOUSE_GACHIBOWLI", "Upload encrypted drive at Gachibowli Hub", FVector(-340000.0f, -175000.0f, 950.0f), 2500.0f, false});
    Missions[M3.MissionId] = M3;

    // Mission 4: Golconda District
    FIUMissionDefinition M4;
    M4.MissionId = "M04_GolcondaSiege";
    M4.Title = "Echoes of the Citadel";
    M4.RewardCurrencyINR = 40000;
    M4.RewardXP = 3500;
    M4.Objectives.push_back({"OBJ_01_FORT_GATE", "Secure the Golconda Outer Rampart Gate", FVector(-198000.0f, -82000.0f, 1100.0f), 2500.0f, false});
    M4.Objectives.push_back({"OBJ_02_GRANITE_RIDGE", "Neutralize Snipers along Deccan Boulder Ridge", FVector(-215000.0f, -65000.0f, 2400.0f), 2500.0f, false});
    M4.Objectives.push_back({"OBJ_03_BALA_HISAR", "Secure the Bala Hisar Summit Pavilion", FVector(-190000.0f, -45000.0f, 3800.0f), 2500.0f, false});
    Missions[M4.MissionId] = M4;

    // Mission 5: Airport District
    FIUMissionDefinition M5;
    M5.MissionId = "M05_ExpresswayIntercept";
    M5.Title = "High-Velocity PVNR Intercept";
    M5.RewardCurrencyINR = 50000;
    M5.RewardXP = 4500;
    M5.Objectives.push_back({"OBJ_01_ONRAMP", "Intercept the Armored Convoy on PVNR Expressway", FVector(-45000.0f, 140000.0f, 1200.0f), 3000.0f, false});
    M5.Objectives.push_back({"OBJ_02_CARGO_APRON", "Breach the Shamshabad Terminal Cargo Apron", FVector(-60000.0f, 320000.0f, 400.0f), 3500.0f, false});
    Missions[M5.MissionId] = M5;

    // Mission 6: Outer Hyderabad Finale
    FIUMissionDefinition M6;
    M6.MissionId = "M06_DeccanShowdown";
    M6.Title = "Crown of the Deccan";
    M6.RewardCurrencyINR = 100000;
    M6.RewardXP = 8000;
    M6.Objectives.push_back({"OBJ_01_VILLAGE_RALLY", "Regroup with Squad at Pochampally Village", FVector(210000.0f, 95000.0f, 300.0f), 3000.0f, false});
    M6.Objectives.push_back({"OBJ_02_FOREST_AMBUSH", "Clear Syndicate Outpost in Ananthagiri Reserve", FVector(-410000.0f, 40000.0f, 4200.0f), 3500.0f, false});
    M6.Objectives.push_back({"OBJ_03_ORR_EXTRACTION", "Secure Final Extraction at Outer Ring Road Toll Plaza", FVector(165000.0f, 240000.0f, 500.0f), 3500.0f, false});
    Missions[M6.MissionId] = M6;
}

bool UIUMissionSubsystem::ServerStartMission(const FString& MissionId)
{
    auto It = Missions.find(MissionId);
    if (It == Missions.end() || It->second.State != EIUMissionState::NotStarted)
    {
        return false;
    }
    It->second.State = EIUMissionState::InProgress;
    It->second.CurrentObjectiveIndex = 0;
    return true;
}

bool UIUMissionSubsystem::ServerUpdateMissionProgress(const FString& MissionId, const FVector& PlayerLocation, AIUPlayerState* AuthoritativePlayerState)
{
    if (AuthoritativePlayerState == nullptr || !AuthoritativePlayerState->HasAuthority())
    {
        return false;
    }

    auto It = Missions.find(MissionId);
    if (It == Missions.end() || It->second.State != EIUMissionState::InProgress)
    {
        return false;
    }

    FIUMissionDefinition& M = It->second;
    if (M.CurrentObjectiveIndex < 0 || M.CurrentObjectiveIndex >= static_cast<int32>(M.Objectives.size()))
    {
        return false;
    }

    FIUMissionObjective& Obj = M.Objectives[static_cast<size_t>(M.CurrentObjectiveIndex)];
    const float Dist = FVector::Dist(PlayerLocation, Obj.TargetLocation);
    if (Dist <= Obj.CompletionRadiusCm)
    {
        Obj.bIsCompleted = true;
        M.CurrentObjectiveIndex += 1;

        if (M.CurrentObjectiveIndex >= static_cast<int32>(M.Objectives.size()))
        {
            M.State = EIUMissionState::Completed;
            AuthoritativePlayerState->ServerAddCurrency(M.RewardCurrencyINR, "MissionComplete_" + MissionId);
            AuthoritativePlayerState->ServerAddExperience(M.RewardXP, "MissionComplete_" + MissionId);
        }
        return true;
    }

    return false;
}

EIUMissionState UIUMissionSubsystem::GetMissionState(const FString& MissionId) const
{
    auto It = Missions.find(MissionId);
    return (It != Missions.end()) ? It->second.State : EIUMissionState::NotStarted;
}

const FIUMissionDefinition* UIUMissionSubsystem::GetMission(const FString& MissionId) const
{
    auto It = Missions.find(MissionId);
    return (It != Missions.end()) ? &(It->second) : nullptr;
}

// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.

#include "../Public/IUBRGameMode.h"
#include <set>

AIUBRGameMode::AIUBRGameMode()
{
    Phases.push_back({1, FVector(0.0f, -145000.0f, 0.0f), 250000.0f, 2.0f});
    Phases.push_back({2, FVector(0.0f, -145000.0f, 0.0f), 120000.0f, 5.0f});
    Phases.push_back({3, FVector(0.0f, -145000.0f, 0.0f),  45000.0f, 10.0f});
    Phases.push_back({4, FVector(0.0f, -145000.0f, 0.0f),  12000.0f, 18.0f});
}

bool AIUBRGameMode::RegisterCompetitor(const FString& AccountId, AIUCharacter* Char, AIUPlayerState* PS, int32 AssignedSquadId)
{
    if (AccountId.empty() || !Char || !PS || static_cast<int32>(Competitors.size()) >= TargetMaxPlayers)
    {
        return false;
    }
    FIUBRCompetitorEntry Entry;
    Entry.AccountId = AccountId;
    Entry.SquadId = AssignedSquadId;
    Entry.Character = Char;
    Entry.PlayerState = PS;
    Entry.DropPhase = EIUBRDropPhase::InAircraft;
    PS->SquadId = AssignedSquadId;
    Competitors[AccountId] = Entry;
    return true;
}

void AIUBRGameMode::StartBattleRoyaleMatch(EIUBRMatchQueueType InQueueType)
{
    QueueType = InQueueType;
    MatchState = EIUBRMatchState::AircraftDropInFlight;
    GenerateHyderabadLootSpawns();

    for (auto& Pair : Competitors)
    {
        Pair.second.DropPhase = EIUBRDropPhase::InAircraft;
        if (Pair.second.Character)
        {
            Pair.second.Character->SetActorLocation(AircraftLocation);
        }
    }
}

void AIUBRGameMode::GenerateHyderabadLootSpawns()
{
    WorldLootTable.clear();
    WorldLootTable.push_back({"LOOT_HYD_01", "WPN_AR_VAJRA", "Weapon", 2, FVector(350.0f, 200.0f, 0.0f), false});
    WorldLootTable.push_back({"LOOT_HYD_02", "ARMOR_VEST_LVL3", "Armor", 3, FVector(420.0f, -180.0f, 0.0f), false});
    WorldLootTable.push_back({"LOOT_HYD_03", "MEDKIT_FULL", "Heal", 2, FVector(-8500.0f, 220.0f, 0.0f), false});
    WorldLootTable.push_back({"LOOT_HYD_04", "WPN_SR_GOLCONDA", "Weapon", 3, FVector(-190000.0f, -45000.0f, 3800.0f), false});
    WorldLootTable.push_back({"LOOT_HYD_05", "ARMOR_HELMET_LVL3", "Armor", 3, FVector(-280000.0f, -210000.0f, 800.0f), false});
}

bool AIUBRGameMode::ServerPickupLootItem(const FString& AccountId, const FString& LootInstanceId)
{
    auto CompIt = Competitors.find(AccountId);
    if (CompIt == Competitors.end() || CompIt->second.bIsEliminated || !CompIt->second.Character)
    {
        return false;
    }

    for (auto& Item : WorldLootTable)
    {
        if (Item.LootInstanceId == LootInstanceId && !Item.bAlreadyPickedUp)
        {
            const float Dist = FVector::Dist(CompIt->second.Character->GetActorLocation(), Item.WorldLocation);
            if (Dist > 400.0f)
            {
                return false; // Too far from loot crate
            }
            Item.bAlreadyPickedUp = true;
            if (Item.Category == "Armor")
            {
                CompIt->second.Character->ServerRestoreArmor(100.0f);
            }
            else if (Item.Category == "Heal")
            {
                CompIt->second.Character->ServerHeal(100.0f);
            }
            return true;
        }
    }
    return false;
}

bool AIUBRGameMode::ServerJumpFromAircraft(const FString& AccountId)
{
    auto It = Competitors.find(AccountId);
    if (It == Competitors.end() || It->second.DropPhase != EIUBRDropPhase::InAircraft)
    {
        return false;
    }
    It->second.DropPhase = EIUBRDropPhase::SkydivingFreefall;
    if (It->second.Character)
    {
        It->second.Character->SetActorLocation(AircraftLocation);
    }
    return true;
}

bool AIUBRGameMode::ServerDeployParachute(const FString& AccountId)
{
    auto It = Competitors.find(AccountId);
    if (It == Competitors.end() || It->second.DropPhase != EIUBRDropPhase::SkydivingFreefall)
    {
        return false;
    }
    It->second.DropPhase = EIUBRDropPhase::ParachuteGliding;
    return true;
}

void AIUBRGameMode::TickParachuteAndAircraft(float DeltaSeconds)
{
    if (DeltaSeconds <= 0.0f)
    {
        return;
    }

    if (MatchState == EIUBRMatchState::AircraftDropInFlight)
    {
        AircraftLocation += (AircraftVelocity * DeltaSeconds);
    }

    for (auto& Pair : Competitors)
    {
        FIUBRCompetitorEntry& Comp = Pair.second;
        if (!Comp.Character || Comp.bIsEliminated)
        {
            continue;
        }

        if (Comp.DropPhase == EIUBRDropPhase::InAircraft)
        {
            Comp.Character->SetActorLocation(AircraftLocation);
        }
        else if (Comp.DropPhase == EIUBRDropPhase::SkydivingFreefall)
        {
            FVector Loc = Comp.Character->GetActorLocation();
            Loc.Z -= 5200.0f * DeltaSeconds; // 52 m/s terminal velocity freefall
            // Auto-open parachute at 150m (15,000 cm) altitude
            if (Loc.Z <= 15000.0f)
            {
                Comp.DropPhase = EIUBRDropPhase::ParachuteGliding;
            }
            Comp.Character->SetActorLocation(Loc);
        }
        else if (Comp.DropPhase == EIUBRDropPhase::ParachuteGliding)
        {
            FVector Loc = Comp.Character->GetActorLocation();
            Loc.Z -= 680.0f * DeltaSeconds;  // 6.8 m/s canopy descent
            Loc.X += 1100.0f * DeltaSeconds; // Forward glide
            if (Loc.Z <= 0.0f)
            {
                Loc.Z = 0.0f;
                Comp.DropPhase = EIUBRDropPhase::LandedOnGround;
                MatchState = EIUBRMatchState::ActiveZoneCombat;
            }
            Comp.Character->SetActorLocation(Loc);
        }
    }
}

bool AIUBRGameMode::IsLocationInsideSafeZone(const FVector& WorldLocation) const
{
    return FVector::Dist2D(WorldLocation, CurrentZoneCenter) <= CurrentZoneRadiusCm;
}

void AIUBRGameMode::TickSafeZoneShrink(float DeltaSeconds, float ShrinkSpeedCmPerSec)
{
    if (CurrentPhaseIndex < 0 || CurrentPhaseIndex >= static_cast<int32>(Phases.size()))
    {
        return;
    }
    const float TargetRadius = Phases[static_cast<size_t>(CurrentPhaseIndex)].TargetRadiusCm;
    if (CurrentZoneRadiusCm > TargetRadius)
    {
        CurrentZoneRadiusCm = FMath::Max(TargetRadius, CurrentZoneRadiusCm - (ShrinkSpeedCmPerSec * DeltaSeconds));
    }
}

float AIUBRGameMode::ApplyZoneDamageIfOutside(AIUCharacter* PlayerChar, float DeltaSeconds)
{
    if (!HasAuthority() || PlayerChar == nullptr || PlayerChar->bIsDead || DeltaSeconds <= 0.0f)
    {
        return 0.0f;
    }
    if (IsLocationInsideSafeZone(PlayerChar->GetActorLocation()))
    {
        return 0.0f;
    }
    const size_t Idx = static_cast<size_t>(FMath::Clamp(CurrentPhaseIndex, 0, static_cast<int32>(Phases.size()) - 1));
    const float DPS = Phases[Idx].DamagePerSecondOutside;
    return PlayerChar->ServerApplyDamage(DPS * DeltaSeconds, false, nullptr, 1e9f);
}

bool AIUBRGameMode::ServerHandlePlayerZeroHealth(const FString& VictimAccountId, const FString& AttackerAccountId)
{
    auto It = Competitors.find(VictimAccountId);
    if (It == Competitors.end() || It->second.bIsEliminated)
    {
        return false;
    }

    FIUBRCompetitorEntry& Victim = It->second;

    // Check if Victim has any alive, non-knocked teammate in Duo/Squad mode
    bool bHasStandingSquadmate = false;
    FString StandingSquadmateId;
    if (QueueType != EIUBRMatchQueueType::Solo)
    {
        for (const auto& Pair : Competitors)
        {
            if (Pair.first != VictimAccountId &&
                Pair.second.SquadId == Victim.SquadId &&
                !Pair.second.bIsEliminated &&
                !Pair.second.bIsKnockedDownDBNO)
            {
                bHasStandingSquadmate = true;
                StandingSquadmateId = Pair.first;
                break;
            }
        }
    }

    if (bHasStandingSquadmate && !Victim.bIsKnockedDownDBNO)
    {
        // Enter DBNO (Down But Not Out) state waiting for revive
        Victim.bIsKnockedDownDBNO = true;
        return true;
    }

    // Eliminate Victim (and any already-knocked squadmates if this was the last standing member)
    const int32 Placement = GetAlivePlayersCount();
    Victim.bIsKnockedDownDBNO = false;
    Victim.bIsEliminated = true;
    Victim.FinalPlacementRank = Placement;
    Victim.SpectatingAccountId = !StandingSquadmateId.empty() ? StandingSquadmateId : AttackerAccountId;

    if (GetAliveSquadsCount() <= 1)
    {
        MatchState = EIUBRMatchState::MatchCompleted;
        for (auto& Pair : Competitors)
        {
            if (!Pair.second.bIsEliminated)
            {
                Pair.second.FinalPlacementRank = 1;
                if (Pair.second.PlayerState && Pair.second.PlayerState->HasAuthority())
                {
                    Pair.second.PlayerState->ServerAddCurrency(35000, "BattleRoyaleVictory");
                    Pair.second.PlayerState->ServerAddExperience(3000, "BattleRoyaleVictory");
                }
            }
        }
    }

    return true;
}

bool AIUBRGameMode::ServerReviveKnockedSquadmate(const FString& ReviverAccountId, const FString& KnockedAccountId)
{
    auto RevIt = Competitors.find(ReviverAccountId);
    auto KnockIt = Competitors.find(KnockedAccountId);
    if (RevIt == Competitors.end() || KnockIt == Competitors.end())
    {
        return false;
    }

    FIUBRCompetitorEntry& Reviver = RevIt->second;
    FIUBRCompetitorEntry& Knocked = KnockIt->second;

    if (Reviver.SquadId != Knocked.SquadId || Reviver.bIsEliminated || Reviver.bIsKnockedDownDBNO || !Knocked.bIsKnockedDownDBNO)
    {
        return false;
    }

    if (Reviver.Character && Knocked.Character)
    {
        if (FVector::Dist(Reviver.Character->GetActorLocation(), Knocked.Character->GetActorLocation()) > 350.0f)
        {
            return false; // Must be within 3.5m to revive
        }
        Knocked.Character->bIsDead = false;
        Knocked.Character->Attributes.CurrentHealth = 30.0f; // Revive with 30 HP
    }

    Knocked.bIsKnockedDownDBNO = false;
    return true;
}

int32 AIUBRGameMode::GetAlivePlayersCount() const
{
    int32 Count = 0;
    for (const auto& Pair : Competitors)
    {
        if (!Pair.second.bIsEliminated)
        {
            ++Count;
        }
    }
    return Count;
}

int32 AIUBRGameMode::GetAliveSquadsCount() const
{
    std::set<int32> AliveSquads;
    for (const auto& Pair : Competitors)
    {
        if (!Pair.second.bIsEliminated)
        {
            AliveSquads.insert(Pair.second.SquadId);
        }
    }
    return static_cast<int32>(AliveSquads.size());
}

const FIUBRCompetitorEntry* AIUBRGameMode::GetCompetitor(const FString& AccountId) const
{
    auto It = Competitors.find(AccountId);
    return (It != Competitors.end()) ? &(It->second) : nullptr;
}

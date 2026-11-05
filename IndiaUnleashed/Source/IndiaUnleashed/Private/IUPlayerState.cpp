// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.

#include "../Public/IUPlayerState.h"

AIUPlayerState::AIUPlayerState()
{
    bReplicates = true;
    bAlwaysRelevant = true;
    AccountId = "iu_acct_default";
}

void AIUPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    APlayerState::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AIUPlayerState, AccountId);
    DOREPLIFETIME(AIUPlayerState, WantedLevel);
    DOREPLIFETIME(AIUPlayerState, PoliceHeatScore);
    DOREPLIFETIME(AIUPlayerState, CurrencyINR);
    DOREPLIFETIME(AIUPlayerState, ExperienceXP);
    DOREPLIFETIME(AIUPlayerState, PlayerLevel);
    DOREPLIFETIME(AIUPlayerState, Kills);
    DOREPLIFETIME(AIUPlayerState, Deaths);
    DOREPLIFETIME(AIUPlayerState, SquadId);
    DOREPLIFETIME(AIUPlayerState, CurrentDistrict);
    DOREPLIFETIME(AIUPlayerState, bIsConnected);
}

bool AIUPlayerState::ServerAddCurrency(int64 DeltaAmount, const FString& ReasonCode)
{
    if (!HasAuthority() || ReasonCode.empty())
    {
        return false;
    }
    if (DeltaAmount < 0 && (CurrencyINR + DeltaAmount) < 0)
    {
        return false; // Insufficient funds
    }
    CurrencyINR += DeltaAmount;
    return true;
}

bool AIUPlayerState::ServerAddExperience(int64 DeltaXP, const FString& SourceTag)
{
    if (!HasAuthority() || DeltaXP <= 0 || SourceTag.empty())
    {
        return false;
    }
    ExperienceXP += DeltaXP;
    // Every 1,000 XP increases player level
    PlayerLevel = 1 + static_cast<int32>(ExperienceXP / 1000);
    return true;
}

bool AIUPlayerState::ServerRecordKill(int32 VictimPlayerId)
{
    if (!HasAuthority() || VictimPlayerId == GetPlayerId())
    {
        return false;
    }
    Kills += 1;
    ServerAddExperience(250, "CombatKill");
    return true;
}

bool AIUPlayerState::ServerRecordDeath()
{
    if (!HasAuthority())
    {
        return false;
    }
    Deaths += 1;
    return true;
}

bool AIUPlayerState::ServerAddPoliceHeat(float HeatDelta)
{
    if (!HasAuthority() || HeatDelta <= 0.0f)
    {
        return false;
    }
    PoliceHeatScore = FMath::Clamp(PoliceHeatScore + HeatDelta, 0.0f, 500.0f);

    if (PoliceHeatScore >= 400.0f) WantedLevel = 5;
    else if (PoliceHeatScore >= 280.0f) WantedLevel = 4;
    else if (PoliceHeatScore >= 175.0f) WantedLevel = 3;
    else if (PoliceHeatScore >= 90.0f) WantedLevel = 2;
    else if (PoliceHeatScore >= 25.0f) WantedLevel = 1;
    else WantedLevel = 0;

    return true;
}

void AIUPlayerState::ServerDecayPoliceHeat(float DeltaSeconds, bool bInPoliceLineOfSight)
{
    if (!HasAuthority() || bInPoliceLineOfSight || PoliceHeatScore <= 0.0f)
    {
        return;
    }
    const float DecayRate = 12.0f; // Heat points per second when out of sight
    PoliceHeatScore = FMath::Max(0.0f, PoliceHeatScore - (DecayRate * DeltaSeconds));

    if (PoliceHeatScore < 25.0f) WantedLevel = 0;
    else if (PoliceHeatScore < 90.0f) WantedLevel = 1;
    else if (PoliceHeatScore < 175.0f) WantedLevel = 2;
    else if (PoliceHeatScore < 280.0f) WantedLevel = 3;
    else if (PoliceHeatScore < 400.0f) WantedLevel = 4;
}

void AIUPlayerState::ServerClearWantedLevel()
{
    if (HasAuthority())
    {
        PoliceHeatScore = 0.0f;
        WantedLevel = 0;
    }
}

void AIUPlayerState::ServerSetCurrentDistrict(const FString& NewDistrict)
{
    if (HasAuthority() && !NewDistrict.empty())
    {
        CurrentDistrict = NewDistrict;
    }
}

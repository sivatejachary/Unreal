// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.

#include "../Public/IUGameState.h"

AIUGameState::AIUGameState()
{
    bReplicates = true;
    bAlwaysRelevant = true;
}

void AIUGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    AGameStateBase::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AIUGameState, TimeOfDayHours);
    DOREPLIFETIME(AIUGameState, CurrentWeather);
    DOREPLIFETIME(AIUGameState, ActiveGameModeId);
    DOREPLIFETIME(AIUGameState, ConnectedPlayersCount);
}

void AIUGameState::ServerAdvanceWorldTime(float RealDeltaSeconds, float DayLengthRealMinutes)
{
    if (!HasAuthority() || RealDeltaSeconds <= 0.0f || DayLengthRealMinutes <= 0.1f)
    {
        return;
    }
    const float GameHoursPerRealSecond = 24.0f / (DayLengthRealMinutes * 60.0f);
    TimeOfDayHours += RealDeltaSeconds * GameHoursPerRealSecond;
    while (TimeOfDayHours >= 24.0f)
    {
        TimeOfDayHours -= 24.0f;
    }
}

void AIUGameState::ServerSetWeather(EIUHyderabadWeather NewWeather)
{
    if (HasAuthority())
    {
        CurrentWeather = NewWeather;
    }
}

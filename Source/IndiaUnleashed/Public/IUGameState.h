// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.
// Replicated World GameState tracking Hyderabad Time-of-Day, Weather, and Active Players.

#pragma once

#include "IUUnrealCoreCompat.h"

UENUM(BlueprintType)
enum class EIUHyderabadWeather : uint8
{
    ClearDeccanSun = 0,
    PartlyCloudy = 1,
    MonsoonDownpour = 2,
    EveningHaze = 3,
    DustSquall = 4
};

UCLASS()
class INDIAUNLEASHED_API AIUGameState : public AGameStateBase
{
    GENERATED_BODY()

public:
    AIUGameState();

    UPROPERTY(Replicated, BlueprintReadOnly, Category="IU|WorldState")
    float TimeOfDayHours = 16.5f; // Default 4:30 PM golden hour over Charminar

    UPROPERTY(Replicated, BlueprintReadOnly, Category="IU|WorldState")
    EIUHyderabadWeather CurrentWeather = EIUHyderabadWeather::ClearDeccanSun;

    UPROPERTY(Replicated, BlueprintReadOnly, Category="IU|WorldState")
    FString ActiveGameModeId = "OpenWorld_Hyderabad_Alpha01";

    UPROPERTY(Replicated, BlueprintReadOnly, Category="IU|WorldState")
    int32 ConnectedPlayersCount = 0;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    void ServerAdvanceWorldTime(float RealDeltaSeconds, float DayLengthRealMinutes = 48.0f);
    void ServerSetWeather(EIUHyderabadWeather NewWeather);
};

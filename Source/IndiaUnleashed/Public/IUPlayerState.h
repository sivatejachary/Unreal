// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.
// Server-Authoritative Replicated PlayerState for Open World, Multiplayer City,
// and Battle Royale modes. Clients CANNOT directly mutate currency, XP, kills, or Wanted level.

#pragma once

#include "IUUnrealCoreCompat.h"

UCLASS()
class INDIAUNLEASHED_API AIUPlayerState : public APlayerState
{
    GENERATED_BODY()

public:
    AIUPlayerState();

    // Replicated Properties
    UPROPERTY(Replicated, BlueprintReadOnly, Category="IU|PlayerState")
    FString AccountId;

    UPROPERTY(Replicated, BlueprintReadOnly, Category="IU|PlayerState")
    int32 WantedLevel = 0; // 0 to 5 stars

    UPROPERTY(Replicated, BlueprintReadOnly, Category="IU|PlayerState")
    float PoliceHeatScore = 0.0f;

    UPROPERTY(Replicated, BlueprintReadOnly, Category="IU|PlayerState")
    int64 CurrencyINR = 5000;

    UPROPERTY(Replicated, BlueprintReadOnly, Category="IU|PlayerState")
    int64 ExperienceXP = 0;

    UPROPERTY(Replicated, BlueprintReadOnly, Category="IU|PlayerState")
    int32 PlayerLevel = 1;

    UPROPERTY(Replicated, BlueprintReadOnly, Category="IU|PlayerState")
    int32 Kills = 0;

    UPROPERTY(Replicated, BlueprintReadOnly, Category="IU|PlayerState")
    int32 Deaths = 0;

    UPROPERTY(Replicated, BlueprintReadOnly, Category="IU|PlayerState")
    int32 SquadId = 0;

    UPROPERTY(Replicated, BlueprintReadOnly, Category="IU|PlayerState")
    FString CurrentDistrict = "OldCity_Charminar";

    UPROPERTY(Replicated, BlueprintReadOnly, Category="IU|PlayerState")
    bool bIsConnected = true;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // Server-Authoritative Mutators (rejected if !HasAuthority())
    bool ServerAddCurrency(int64 DeltaAmount, const FString& ReasonCode);
    bool ServerAddExperience(int64 DeltaXP, const FString& SourceTag);
    bool ServerRecordKill(int32 VictimPlayerId);
    bool ServerRecordDeath();
    bool ServerAddPoliceHeat(float HeatDelta);
    void ServerDecayPoliceHeat(float DeltaSeconds, bool bInPoliceLineOfSight);
    void ServerClearWantedLevel();
    void ServerSetCurrentDistrict(const FString& NewDistrict);
};

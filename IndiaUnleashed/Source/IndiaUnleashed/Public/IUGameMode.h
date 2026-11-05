// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.
// Authoritative Server GameMode for Hyderabad Open World & 2-Player Listen/Dedicated Server Prototype.

#pragma once

#include "IUUnrealCoreCompat.h"
#include "IUCharacter.h"
#include "IUPlayerController.h"
#include "IUPlayerState.h"
#include "IUGameState.h"

struct FIUDisconnectedPlayerSnapshot
{
    FString AccountId;
    FTransform LastTransform;
    int64 CurrencyINR = 5000;
    int64 ExperienceXP = 0;
    int32 WantedLevel = 0;
};

UCLASS()
class INDIAUNLEASHED_API AIUGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AIUGameMode();
    virtual ~AIUGameMode();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="IU|GameMode")
    int32 MaxSessionPlayers = 16; // Configured for 2-player Alpha 0.1 listen server up to 16-player city

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="IU|GameMode")
    TArray<FTransform> CharminarSpawnPoints;

    AIUGameState* GetIUGameState() const { return IUGameState; }

    bool HandlePlayerLogin(AIUPlayerController* NewPC, const FString& AccountId, const FString& DisplayName);
    bool HandlePlayerDisconnect(AIUPlayerController* LeavingPC);
    FTransform SelectSpawnTransform(int32 PlayerIndex) const;

private:
    AIUGameState* IUGameState = nullptr;
    TMap<FString, FIUDisconnectedPlayerSnapshot> DisconnectedCache;
    int32 NextAssignedPlayerId = 1001;
};

// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.
// Online Session & Party Subsystem handling Listen Server (2-player Alpha 0.1),
// Dedicated Server (Multiplayer City & 100-player Battle Royale), Disconnect/Reconnect,
// and FastAPI Backend HMAC-Signed Request Construction.

#pragma once

#include "../../IndiaUnleashed/Public/IUUnrealCoreCompat.h"

UENUM(BlueprintType)
enum class EIUSessionMode : uint8
{
    OpenWorldSoloOrCoop = 0,
    MultiplayerCity = 1,
    BattleRoyaleSolo = 2,
    BattleRoyaleDuo = 3,
    BattleRoyaleSquad = 4
};

USTRUCT(BlueprintType)
struct FIUSessionDescriptor
{
    GENERATED_BODY()

    FString SessionId;
    FString HostAccountId;
    FString MapName = "/Game/Maps/Hyderabad/L_Hyderabad_Prototype";
    EIUSessionMode Mode = EIUSessionMode::OpenWorldSoloOrCoop;
    bool bIsDedicatedServer = false;
    bool bIsLANMatch = false;
    int32 MaxPlayers = 2;
    int32 CurrentPlayers = 0;
    TArray<FString> ConnectedAccountIds;
};

UCLASS()
class IUONLINE_API UIUSessionSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UIUSessionSubsystem();

    bool CreateSession(const FString& HostAccountId, EIUSessionMode Mode, int32 MaxPlayers, bool bDedicated, FString& OutSessionId);
    bool JoinSession(const FString& SessionId, const FString& JoiningAccountId);
    bool LeaveSession(const FString& SessionId, const FString& LeavingAccountId);
    bool FindSessions(EIUSessionMode FilterMode, TArray<FIUSessionDescriptor>& OutResults) const;
    const FIUSessionDescriptor* GetSession(const FString& SessionId) const;

    // Server-to-Backend HMAC Signing (prevents clients from forging reward/currency calls)
    static FString ComputeServerHmacToken(const FString& EndpointPath, const FString& PayloadJson, const FString& ServerSecret);

private:
    TMap<FString, FIUSessionDescriptor> ActiveSessions;
    int32 SessionCounter = 100;
};

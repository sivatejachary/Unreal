// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.
// Mode B: Multiplayer City Systems - Friends & Party Manager, Street Racing Checkpoints,
// and 3D Spatial + Party Voice-Chat Integration Point.

#pragma once

#include "../../IndiaUnleashed/Public/IUUnrealCoreCompat.h"
#include "../../IndiaUnleashed/Public/IUPlayerState.h"

USTRUCT(BlueprintType)
struct FIUPartyState
{
    GENERATED_BODY()

    FString PartyId;
    FString LeaderAccountId;
    int32 MaxMembers = 4;
    TArray<FString> MemberAccountIds;
};

USTRUCT(BlueprintType)
struct FIUStreetRaceParticipant
{
    GENERATED_BODY()

    FString AccountId;
    AIUPlayerState* PlayerState = nullptr;
    int32 NextCheckpointIndex = 0;
    float FinishTimeSeconds = -1.0f;
    bool bFinished = false;
};

UCLASS()
class IUONLINE_API UIUMultiplayerCitySubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UIUMultiplayerCitySubsystem();

    // Friends & Party Management
    bool SendFriendInvite(const FString& FromAccount, const FString& ToAccount);
    bool AcceptFriendInvite(const FString& FromAccount, const FString& ToAccount);
    bool AreFriends(const FString& AccountA, const FString& AccountB) const;

    bool CreateParty(const FString& LeaderAccount, FString& OutPartyId);
    bool JoinParty(const FString& PartyId, const FString& JoiningAccount);
    bool LeaveParty(const FString& PartyId, const FString& LeavingAccount);
    const FIUPartyState* GetParty(const FString& PartyId) const;

    // Multiplayer Street Racing (Tank Bund / Necklace Road Circuit)
    void InitializeNecklaceRoadCircuit();
    bool RegisterRacer(AIUPlayerState* RacerPS);
    bool ServerValidateRaceCheckpoint(const FString& AccountId, const FVector& VehicleLocation, float ElapsedRaceSeconds);
    int32 GetFinishedRacersCount() const { return FinishedCount; }

    // Voice-Chat Integration Point (Proximity 3D + Squad Channel)
    void JoinVoiceChannel(const FString& AccountId, const FString& ChannelName, bool bIs3DPositional);
    void SetPlayerMuted(const FString& LocalAccount, const FString& TargetAccount, bool bMuted);
    bool IsPlayerMuted(const FString& LocalAccount, const FString& TargetAccount) const;
    static float ComputeVoiceSpatialAttenuation(const FVector& SpeakerPos, const FVector& ListenerPos, float MaxAudibleDistCm = 2500.0f);

private:
    TMap<FString, TArray<FString>> FriendsGraph;
    TMap<FString, FIUPartyState> Parties;
    TArray<FVector> RaceCheckpoints;
    TMap<FString, FIUStreetRaceParticipant> ActiveRacers;
    TMap<FString, TArray<FString>> MutedPlayersMap;
    TMap<FString, FString> ActiveVoiceChannels;
    int32 PartySeq = 1;
    int32 FinishedCount = 0;
};

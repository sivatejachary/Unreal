// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.

#include "../Public/IUMultiplayerCitySystems.h"

UIUMultiplayerCitySubsystem::UIUMultiplayerCitySubsystem()
{
    InitializeNecklaceRoadCircuit();
}

bool UIUMultiplayerCitySubsystem::SendFriendInvite(const FString& FromAccount, const FString& ToAccount)
{
    return (!FromAccount.empty() && !ToAccount.empty() && FromAccount != ToAccount);
}

bool UIUMultiplayerCitySubsystem::AcceptFriendInvite(const FString& FromAccount, const FString& ToAccount)
{
    if (FromAccount.empty() || ToAccount.empty() || FromAccount == ToAccount)
    {
        return false;
    }
    FriendsGraph[FromAccount].push_back(ToAccount);
    FriendsGraph[ToAccount].push_back(FromAccount);
    return true;
}

bool UIUMultiplayerCitySubsystem::AreFriends(const FString& AccountA, const FString& AccountB) const
{
    auto It = FriendsGraph.find(AccountA);
    if (It == FriendsGraph.end())
    {
        return false;
    }
    const auto& Vec = It->second;
    return std::find(Vec.begin(), Vec.end(), AccountB) != Vec.end();
}

bool UIUMultiplayerCitySubsystem::CreateParty(const FString& LeaderAccount, FString& OutPartyId)
{
    if (LeaderAccount.empty())
    {
        return false;
    }
    FIUPartyState P;
    P.PartyId = "party_hyd_" + std::to_string(PartySeq++);
    P.LeaderAccountId = LeaderAccount;
    P.MemberAccountIds.push_back(LeaderAccount);
    OutPartyId = P.PartyId;
    Parties[P.PartyId] = P;
    return true;
}

bool UIUMultiplayerCitySubsystem::JoinParty(const FString& PartyId, const FString& JoiningAccount)
{
    auto It = Parties.find(PartyId);
    if (It == Parties.end() || JoiningAccount.empty())
    {
        return false;
    }
    FIUPartyState& P = It->second;
    if (static_cast<int32>(P.MemberAccountIds.size()) >= P.MaxMembers)
    {
        return false;
    }
    if (std::find(P.MemberAccountIds.begin(), P.MemberAccountIds.end(), JoiningAccount) == P.MemberAccountIds.end())
    {
        P.MemberAccountIds.push_back(JoiningAccount);
    }
    return true;
}

bool UIUMultiplayerCitySubsystem::LeaveParty(const FString& PartyId, const FString& LeavingAccount)
{
    auto It = Parties.find(PartyId);
    if (It == Parties.end())
    {
        return false;
    }
    auto& Vec = It->second.MemberAccountIds;
    Vec.erase(std::remove(Vec.begin(), Vec.end(), LeavingAccount), Vec.end());
    if (Vec.empty())
    {
        Parties.erase(It);
    }
    return true;
}

const FIUPartyState* UIUMultiplayerCitySubsystem::GetParty(const FString& PartyId) const
{
    auto It = Parties.find(PartyId);
    return (It != Parties.end()) ? &(It->second) : nullptr;
}

void UIUMultiplayerCitySubsystem::InitializeNecklaceRoadCircuit()
{
    RaceCheckpoints.clear();
    RaceCheckpoints.push_back(FVector(18000.0f, -130000.0f, 200.0f)); // CP 0: Tank Bund South
    RaceCheckpoints.push_back(FVector(5000.0f,  -142000.0f, 180.0f)); // CP 1: Lumbini Waterfront
    RaceCheckpoints.push_back(FVector(-12000.0f,-152000.0f, 160.0f)); // CP 2: Necklace Road Curve
    RaceCheckpoints.push_back(FVector(-22000.0f,-158000.0f, 150.0f)); // CP 3: Finish Gate
    ActiveRacers.clear();
    FinishedCount = 0;
}

bool UIUMultiplayerCitySubsystem::RegisterRacer(AIUPlayerState* RacerPS)
{
    if (!RacerPS || RacerPS->AccountId.empty())
    {
        return false;
    }
    FIUStreetRaceParticipant Entry;
    Entry.AccountId = RacerPS->AccountId;
    Entry.PlayerState = RacerPS;
    Entry.NextCheckpointIndex = 0;
    ActiveRacers[Entry.AccountId] = Entry;
    return true;
}

bool UIUMultiplayerCitySubsystem::ServerValidateRaceCheckpoint(const FString& AccountId, const FVector& VehicleLocation, float ElapsedRaceSeconds)
{
    auto It = ActiveRacers.find(AccountId);
    if (It == ActiveRacers.end() || It->second.bFinished || RaceCheckpoints.empty())
    {
        return false;
    }

    FIUStreetRaceParticipant& Racer = It->second;
    const FVector TargetCP = RaceCheckpoints[static_cast<size_t>(Racer.NextCheckpointIndex)];
    if (FVector::Dist(VehicleLocation, TargetCP) <= 2200.0f) // 22m gate width
    {
        Racer.NextCheckpointIndex += 1;
        if (Racer.NextCheckpointIndex >= static_cast<int32>(RaceCheckpoints.size()))
        {
            Racer.bFinished = true;
            Racer.FinishTimeSeconds = ElapsedRaceSeconds;
            FinishedCount += 1;

            // Award 1st / 2nd / 3rd place prize money authoritatively
            if (Racer.PlayerState && Racer.PlayerState->HasAuthority())
            {
                const int64 PrizeINR = (FinishedCount == 1) ? 25000 : ((FinishedCount == 2) ? 12000 : 5000);
                Racer.PlayerState->ServerAddCurrency(PrizeINR, "StreetRacePodium");
                Racer.PlayerState->ServerAddExperience(1000, "StreetRaceFinish");
            }
        }
        return true;
    }
    return false;
}

void UIUMultiplayerCitySubsystem::JoinVoiceChannel(const FString& AccountId, const FString& ChannelName, bool bIs3DPositional)
{
    (void)bIs3DPositional;
    if (!AccountId.empty() && !ChannelName.empty())
    {
        ActiveVoiceChannels[AccountId] = ChannelName;
    }
}

void UIUMultiplayerCitySubsystem::SetPlayerMuted(const FString& LocalAccount, const FString& TargetAccount, bool bMuted)
{
    auto& Vec = MutedPlayersMap[LocalAccount];
    if (bMuted)
    {
        if (std::find(Vec.begin(), Vec.end(), TargetAccount) == Vec.end())
        {
            Vec.push_back(TargetAccount);
        }
    }
    else
    {
        Vec.erase(std::remove(Vec.begin(), Vec.end(), TargetAccount), Vec.end());
    }
}

bool UIUMultiplayerCitySubsystem::IsPlayerMuted(const FString& LocalAccount, const FString& TargetAccount) const
{
    auto It = MutedPlayersMap.find(LocalAccount);
    if (It == MutedPlayersMap.end())
    {
        return false;
    }
    return std::find(It->second.begin(), It->second.end(), TargetAccount) != It->second.end();
}

float UIUMultiplayerCitySubsystem::ComputeVoiceSpatialAttenuation(const FVector& SpeakerPos, const FVector& ListenerPos, float MaxAudibleDistCm)
{
    const float Dist = FVector::Dist(SpeakerPos, ListenerPos);
    if (Dist >= MaxAudibleDistCm)
    {
        return 0.0f;
    }
    return FMath::Clamp(1.0f - (Dist / MaxAudibleDistCm), 0.0f, 1.0f);
}

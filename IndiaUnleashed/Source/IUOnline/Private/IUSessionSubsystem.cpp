// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.

#include "../Public/IUSessionSubsystem.h"

UIUSessionSubsystem::UIUSessionSubsystem() = default;

bool UIUSessionSubsystem::CreateSession(const FString& HostAccountId, EIUSessionMode Mode, int32 MaxPlayers, bool bDedicated, FString& OutSessionId)
{
    if (HostAccountId.empty() || MaxPlayers <= 0 || MaxPlayers > 100)
    {
        return false;
    }

    FIUSessionDescriptor Desc;
    Desc.SessionId = "iu_sess_hyd_" + std::to_string(++SessionCounter);
    Desc.HostAccountId = HostAccountId;
    Desc.Mode = Mode;
    Desc.bIsDedicatedServer = bDedicated;
    Desc.MaxPlayers = MaxPlayers;

    if (!bDedicated)
    {
        // Listen server host joins immediately as Player 1
        Desc.ConnectedAccountIds.push_back(HostAccountId);
        Desc.CurrentPlayers = 1;
    }

    OutSessionId = Desc.SessionId;
    ActiveSessions[Desc.SessionId] = Desc;
    return true;
}

bool UIUSessionSubsystem::JoinSession(const FString& SessionId, const FString& JoiningAccountId)
{
    if (SessionId.empty() || JoiningAccountId.empty())
    {
        return false;
    }

    auto It = ActiveSessions.find(SessionId);
    if (It == ActiveSessions.end())
    {
        return false;
    }

    FIUSessionDescriptor& Sess = It->second;
    if (Sess.CurrentPlayers >= Sess.MaxPlayers)
    {
        return false; // Session full
    }

    for (const auto& Existing : Sess.ConnectedAccountIds)
    {
        if (Existing == JoiningAccountId)
        {
            return true; // Idempotent reconnect
        }
    }

    Sess.ConnectedAccountIds.push_back(JoiningAccountId);
    Sess.CurrentPlayers = static_cast<int32>(Sess.ConnectedAccountIds.size());
    return true;
}

bool UIUSessionSubsystem::LeaveSession(const FString& SessionId, const FString& LeavingAccountId)
{
    auto It = ActiveSessions.find(SessionId);
    if (It == ActiveSessions.end())
    {
        return false;
    }

    FIUSessionDescriptor& Sess = It->second;
    auto& Vec = Sess.ConnectedAccountIds;
    auto Found = std::find(Vec.begin(), Vec.end(), LeavingAccountId);
    if (Found == Vec.end())
    {
        return false;
    }

    Vec.erase(Found);
    Sess.CurrentPlayers = static_cast<int32>(Vec.size());

    if (Sess.CurrentPlayers == 0 && !Sess.bIsDedicatedServer)
    {
        ActiveSessions.erase(It);
    }
    return true;
}

bool UIUSessionSubsystem::FindSessions(EIUSessionMode FilterMode, TArray<FIUSessionDescriptor>& OutResults) const
{
    OutResults.clear();
    for (const auto& Pair : ActiveSessions)
    {
        if (Pair.second.Mode == FilterMode && Pair.second.CurrentPlayers < Pair.second.MaxPlayers)
        {
            OutResults.push_back(Pair.second);
        }
    }
    return !OutResults.empty();
}

const FIUSessionDescriptor* UIUSessionSubsystem::GetSession(const FString& SessionId) const
{
    auto It = ActiveSessions.find(SessionId);
    return (It != ActiveSessions.end()) ? &(It->second) : nullptr;
}

FString UIUSessionSubsystem::ComputeServerHmacToken(const FString& EndpointPath, const FString& PayloadJson, const FString& ServerSecret)
{
    uint64 Hash = 14695981039346656037ULL;
    const FString Combined = EndpointPath + "|" + PayloadJson + "|" + ServerSecret;
    for (char c : Combined)
    {
        Hash ^= static_cast<uint8>(c);
        Hash *= 1099511628211ULL;
    }
    std::ostringstream Oss;
    Oss << std::hex << std::setw(16) << std::setfill('0') << Hash;
    return Oss.str();
}

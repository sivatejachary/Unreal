// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.

#include "../Public/IUGameMode.h"

AIUGameMode::AIUGameMode()
{
    IUGameState = new AIUGameState();
    GameState = IUGameState;

    // Default spawn transforms around Charminar Plaza (Old City, Hyderabad)
    CharminarSpawnPoints.push_back(FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(-2200.0f, 450.0f, 110.0f)));
    CharminarSpawnPoints.push_back(FTransform(FRotator(0.0f, 90.0f, 0.0f), FVector(2200.0f, -450.0f, 110.0f)));
    CharminarSpawnPoints.push_back(FTransform(FRotator(0.0f, 180.0f, 0.0f), FVector(450.0f, 2200.0f, 110.0f)));
    CharminarSpawnPoints.push_back(FTransform(FRotator(0.0f, 270.0f, 0.0f), FVector(-450.0f, -2200.0f, 110.0f)));
}

AIUGameMode::~AIUGameMode()
{
    delete IUGameState;
    IUGameState = nullptr;
    GameState = nullptr;
}

FTransform AIUGameMode::SelectSpawnTransform(int32 PlayerIndex) const
{
    if (CharminarSpawnPoints.empty())
    {
        return FTransform();
    }
    const size_t Index = static_cast<size_t>( std::abs(PlayerIndex) ) % CharminarSpawnPoints.size();
    return CharminarSpawnPoints[Index];
}

bool AIUGameMode::HandlePlayerLogin(AIUPlayerController* NewPC, const FString& AccountId, const FString& DisplayName)
{
    if (!HasAuthority() || NewPC == nullptr || AccountId.empty())
    {
        return false;
    }
    if (IUGameState->ConnectedPlayersCount >= MaxSessionPlayers)
    {
        return false; // Server full
    }

    auto* NewPS = new AIUPlayerState();
    NewPS->SetPlayerId(NextAssignedPlayerId++);
    NewPS->AccountId = AccountId;
    NewPS->SetPlayerName(DisplayName);
    NewPS->bIsConnected = true;

    auto* NewChar = new AIUCharacter();
    FTransform SpawnTransform = SelectSpawnTransform(IUGameState->ConnectedPlayersCount);

    // Restore snapshot if reconnecting
    auto It = DisconnectedCache.find(AccountId);
    if (It != DisconnectedCache.end())
    {
        SpawnTransform = It->second.LastTransform;
        NewPS->CurrencyINR = It->second.CurrencyINR;
        NewPS->ExperienceXP = It->second.ExperienceXP;
        NewPS->WantedLevel = It->second.WantedLevel;
        DisconnectedCache.erase(It);
    }

    NewChar->SetActorLocation(SpawnTransform.Translation);
    NewChar->SetActorRotation(SpawnTransform.Rotation);

    NewPC->PlayerState = NewPS;
    NewPC->Possess(NewChar);

    IUGameState->PlayerArray.push_back(NewPS);
    IUGameState->ConnectedPlayersCount = static_cast<int32>(IUGameState->PlayerArray.size());
    return true;
}

bool AIUGameMode::HandlePlayerDisconnect(AIUPlayerController* LeavingPC)
{
    if (!HasAuthority() || LeavingPC == nullptr || LeavingPC->PlayerState == nullptr)
    {
        return false;
    }

    auto* LeavingPS = static_cast<AIUPlayerState*>(LeavingPC->PlayerState);
    LeavingPS->bIsConnected = false;

    FIUDisconnectedPlayerSnapshot Snapshot;
    Snapshot.AccountId = LeavingPS->AccountId;
    Snapshot.CurrencyINR = LeavingPS->CurrencyINR;
    Snapshot.ExperienceXP = LeavingPS->ExperienceXP;
    Snapshot.WantedLevel = LeavingPS->WantedLevel;

    if (APawn* Pawn = LeavingPC->GetPawn())
    {
        if (auto* Char = dynamic_cast<AIUCharacter*>(Pawn))
        {
            if (Char->CurrentVehicle != nullptr)
            {
                Char->ServerExitVehicle();
            }
        }
        Snapshot.LastTransform = FTransform(Pawn->GetActorRotation(), Pawn->GetActorLocation());
        LeavingPC->UnPossess();
        delete Pawn;
    }

    DisconnectedCache[LeavingPS->AccountId] = Snapshot;

    auto& Arr = IUGameState->PlayerArray;
    Arr.erase(std::remove(Arr.begin(), Arr.end(), LeavingPS), Arr.end());
    IUGameState->ConnectedPlayersCount = static_cast<int32>(Arr.size());

    delete LeavingPS;
    LeavingPC->PlayerState = nullptr;
    return true;
}

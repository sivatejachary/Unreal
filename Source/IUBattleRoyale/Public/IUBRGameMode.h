// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.
// Mode C: Full Production 100-Player Battle Royale Mode Architecture.
// Implements Solo/Duo/Squad Lobby, Transport Aircraft Drop Sequence, Parachute Physics,
// Tiered Loot Spawning, Shrinking Safe Zone, Duo/Squad DBNO Knockdown & Revive,
// Elimination, Spectator Routing, and Match Results / Leaderboards.

#pragma once

#include "../../IndiaUnleashed/Public/IUUnrealCoreCompat.h"
#include "../../IndiaUnleashed/Public/IUCharacter.h"
#include "../../IndiaUnleashed/Public/IUPlayerState.h"

UENUM(BlueprintType)
enum class EIUBRMatchQueueType : uint8
{
    Solo = 1,
    Duo = 2,
    Squad = 4
};

UENUM(BlueprintType)
enum class EIUBRMatchState : uint8
{
    WaitingLobby = 0,
    PreDropCountdown = 1,
    AircraftDropInFlight = 2,
    ActiveZoneCombat = 3,
    MatchCompleted = 4
};

UENUM(BlueprintType)
enum class EIUBRDropPhase : uint8
{
    InAircraft = 0,
    SkydivingFreefall = 1,
    ParachuteGliding = 2,
    LandedOnGround = 3
};

USTRUCT(BlueprintType)
struct FIUBRSafeZonePhase
{
    GENERATED_BODY()

    int32 PhaseNumber = 1;
    FVector CenterLocation{0.0f, -145000.0f, 0.0f}; // Hussain Sagar Lake center
    float TargetRadiusCm = 250000.0f;
    float DamagePerSecondOutside = 2.0f;
};

USTRUCT(BlueprintType)
struct FIUBRLootSpawnItem
{
    GENERATED_BODY()

    FString LootInstanceId;
    FString ItemId;         // e.g. "WPN_AR_VAJRA", "ARMOR_VEST_LVL3", "MEDKIT_FULL"
    FString Category;       // "Weapon", "Armor", "Heal", "Attachment"
    int32 TierLevel = 1;    // 1, 2, or 3
    FVector WorldLocation{0.0f, 0.0f, 0.0f};
    bool bAlreadyPickedUp = false;
};

USTRUCT(BlueprintType)
struct FIUBRCompetitorEntry
{
    GENERATED_BODY()

    FString AccountId;
    int32 SquadId = 1;
    AIUCharacter* Character = nullptr;
    AIUPlayerState* PlayerState = nullptr;
    EIUBRDropPhase DropPhase = EIUBRDropPhase::InAircraft;
    bool bIsKnockedDownDBNO = false;
    bool bIsEliminated = false;
    int32 FinalPlacementRank = 0; // 1 = Winner, 100 = First out
    FString SpectatingAccountId;
};

UCLASS()
class IUBATTLEROYALE_API AIUBRGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AIUBRGameMode();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="IU|BattleRoyale")
    int32 TargetMaxPlayers = 100;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="IU|BattleRoyale")
    EIUBRMatchQueueType QueueType = EIUBRMatchQueueType::Squad;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IU|BattleRoyale")
    EIUBRMatchState MatchState = EIUBRMatchState::WaitingLobby;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IU|BattleRoyale")
    float CurrentZoneRadiusCm = 400000.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IU|BattleRoyale")
    FVector CurrentZoneCenter{0.0f, -145000.0f, 0.0f};

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IU|BattleRoyale")
    int32 CurrentPhaseIndex = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IU|BattleRoyale")
    FVector AircraftLocation{-300000.0f, -200000.0f, 45000.0f}; // 450m altitude

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IU|BattleRoyale")
    FVector AircraftVelocity{12000.0f, 9000.0f, 0.0f};          // 150 m/s transport plane

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IU|BattleRoyale")
    TArray<FIUBRSafeZonePhase> Phases;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IU|BattleRoyale")
    TArray<FIUBRLootSpawnItem> WorldLootTable;

    // Lobby, Match Start & Loot Generation
    bool RegisterCompetitor(const FString& AccountId, AIUCharacter* Char, AIUPlayerState* PS, int32 AssignedSquadId);
    void StartBattleRoyaleMatch(EIUBRMatchQueueType InQueueType);
    void GenerateHyderabadLootSpawns();
    bool ServerPickupLootItem(const FString& AccountId, const FString& LootInstanceId);

    // Aircraft Drop & Parachute Simulation
    bool ServerJumpFromAircraft(const FString& AccountId);
    bool ServerDeployParachute(const FString& AccountId);
    void TickParachuteAndAircraft(float DeltaSeconds);

    // Safe Zone & Storm Damage
    bool IsLocationInsideSafeZone(const FVector& WorldLocation) const;
    void TickSafeZoneShrink(float DeltaSeconds, float ShrinkSpeedCmPerSec = 1500.0f);
    float ApplyZoneDamageIfOutside(AIUCharacter* PlayerChar, float DeltaSeconds);

    // Knockdown (DBNO), Teammate Revive, Elimination & Spectator Routing
    bool ServerHandlePlayerZeroHealth(const FString& VictimAccountId, const FString& AttackerAccountId);
    bool ServerReviveKnockedSquadmate(const FString& ReviverAccountId, const FString& KnockedAccountId);
    int32 GetAlivePlayersCount() const;
    int32 GetAliveSquadsCount() const;
    const FIUBRCompetitorEntry* GetCompetitor(const FString& AccountId) const;

private:
    TMap<FString, FIUBRCompetitorEntry> Competitors;
};

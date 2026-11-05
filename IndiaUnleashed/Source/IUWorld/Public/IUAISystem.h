// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.
// Full Production NPC, Pedestrian, Traffic Spline & 5-Star Police AI Controllers.

#pragma once

#include "IUHyderabadTypes.h"
#include "../../IndiaUnleashed/Public/IUCharacter.h"
#include "../../IUVehicles/Public/IUVehicleBase.h"

UENUM(BlueprintType)
enum class EIUPedestrianBehaviorState : uint8
{
    SidewalkWander = 0,
    MarketBrowsing = 1,
    ShopkeeperIdle = 2,
    CowerInPlace = 3,
    FleeFromDanger = 4
};

UENUM(BlueprintType)
enum class EIUPoliceTacticState : uint8
{
    RoutinePatrol = 0,
    InvestigateLastKnownPosition = 1,
    BeatOfficerFootPursuit = 2,      // Wanted Star 1
    InterceptorRammingPursuit = 3,   // Wanted Star 2
    RoadblockAndSpikeStrip = 4,      // Wanted Star 3
    TacticalSWATContainment = 5,     // Wanted Star 4
    ApexLockdownWithHeliSupport = 6  // Wanted Star 5
};

UCLASS()
class IUWORLD_API AIUPedestrianController : public AController
{
    GENERATED_BODY()

public:
    AIUPedestrianController();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IU|AI|Pedestrian")
    EIUPedestrianBehaviorState BehaviorState = EIUPedestrianBehaviorState::SidewalkWander;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IU|AI|Pedestrian")
    EIUNPCSimulationTier CurrentLODTier = EIUNPCSimulationTier::Tier0_FullBehaviorTreeAndPhysics;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="IU|AI|Pedestrian")
    float DangerPerceptionRadiusCm = 2800.0f; // 28m

    void NotifyGunfireOrExplosionNearby(const FVector& ThreatOriginLocation);
    void TickPedestrianAI(float DeltaSeconds, const FVector& ThreatOriginLocation, const TArray<FVector>& PlayerLocations);
};

UCLASS()
class IUWORLD_API AIUTrafficVehicleController : public AController
{
    GENERATED_BODY()

public:
    AIUTrafficVehicleController();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IU|AI|Traffic")
    TArray<FVector> RouteSplinePoints;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IU|AI|Traffic")
    int32 CurrentWaypointIndex = 0;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="IU|AI|Traffic")
    float SafeFollowingDistanceCm = 1200.0f; // 12m braking distance

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IU|AI|Traffic")
    bool bStoppedAtRedLightOrObstacle = false;

    void AssignHyderabadTrafficRoute(const TArray<FVector>& InWaypoints);
    void TickTrafficDriving(AIUVehicleBase* ControlledVehicle, const FVector& NearestObstacleAhead, float DeltaSeconds);
};

UCLASS()
class IUWORLD_API AIUPoliceAIController : public AController
{
    GENERATED_BODY()

public:
    AIUPoliceAIController();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IU|AI|Police")
    EIUPoliceTacticState TacticState = EIUPoliceTacticState::RoutinePatrol;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IU|AI|Police")
    FVector LastKnownSuspectLocation{0.0f, 0.0f, 0.0f};

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IU|AI|Police")
    bool bHasLineOfSightToSuspect = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IU|AI|Police")
    bool bSpikeStripDeployed = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IU|AI|Police")
    bool bHelicopterSpotlightLocked = false;

    void EvaluateSuspectPursuit(AIUCharacter* Suspect, AIUPlayerState* SuspectPS, float MaxSightRangeCm = 8500.0f);
    void ExecutePursuitStep(AIUCharacter* PoliceOfficerOrInterceptor, AIUCharacter* Suspect, float DeltaSeconds);
};

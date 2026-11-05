// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.

#include "../Public/IUAISystem.h"
#include "../Public/IUHyderabadWorldSubsystem.h"

AIUPedestrianController::AIUPedestrianController() = default;

void AIUPedestrianController::NotifyGunfireOrExplosionNearby(const FVector& ThreatOriginLocation)
{
    if (!PossessedPawn)
    {
        return;
    }
    const float Dist = FVector::Dist(PossessedPawn->GetActorLocation(), ThreatOriginLocation);
    if (Dist <= DangerPerceptionRadiusCm)
    {
        BehaviorState = EIUPedestrianBehaviorState::FleeFromDanger;
    }
}

void AIUPedestrianController::TickPedestrianAI(float DeltaSeconds, const FVector& ThreatOriginLocation, const TArray<FVector>& PlayerLocations)
{
    if (!PossessedPawn || DeltaSeconds <= 0.0f)
    {
        return;
    }

    CurrentLODTier = UIUHyderabadWorldSubsystem::EvaluateNPCSimulationTier(PossessedPawn->GetActorLocation(), PlayerLocations);
    if (CurrentLODTier == EIUNPCSimulationTier::Tier3_CulledOrDormant)
    {
        return; // Dormant outside 350m radius to preserve CPU budget
    }

    if (BehaviorState == EIUPedestrianBehaviorState::FleeFromDanger)
    {
        const FVector AwayDir = (PossessedPawn->GetActorLocation() - ThreatOriginLocation).GetSafeNormal();
        const float FleeSpeedCmPerSec = 520.0f;
        PossessedPawn->SetActorLocation(PossessedPawn->GetActorLocation() + (AwayDir * FleeSpeedCmPerSec * DeltaSeconds));
    }
    else if (BehaviorState == EIUPedestrianBehaviorState::SidewalkWander)
    {
        const FVector Forward = PossessedPawn->GetActorForwardVector();
        PossessedPawn->SetActorLocation(PossessedPawn->GetActorLocation() + (Forward * 160.0f * DeltaSeconds));
    }
}

AIUTrafficVehicleController::AIUTrafficVehicleController() = default;

void AIUTrafficVehicleController::AssignHyderabadTrafficRoute(const TArray<FVector>& InWaypoints)
{
    RouteSplinePoints = InWaypoints;
    CurrentWaypointIndex = 0;
}

void AIUTrafficVehicleController::TickTrafficDriving(AIUVehicleBase* ControlledVehicle, const FVector& NearestObstacleAhead, float DeltaSeconds)
{
    if (!ControlledVehicle || RouteSplinePoints.empty() || DeltaSeconds <= 0.0f)
    {
        return;
    }

    const float ObstacleDist = FVector::Dist(ControlledVehicle->GetActorLocation(), NearestObstacleAhead);
    if (ObstacleDist <= SafeFollowingDistanceCm)
    {
        bStoppedAtRedLightOrObstacle = true;
        ControlledVehicle->SimulateVehiclePhysics(0.0f, 1.0f, 0.0f, DeltaSeconds);
        return;
    }

    bStoppedAtRedLightOrObstacle = false;
    const FVector TargetWP = RouteSplinePoints[static_cast<size_t>(CurrentWaypointIndex)];
    if (FVector::Dist2D(ControlledVehicle->GetActorLocation(), TargetWP) < 400.0f)
    {
        CurrentWaypointIndex = (CurrentWaypointIndex + 1) % static_cast<int32>(RouteSplinePoints.size());
    }

    ControlledVehicle->SimulateVehiclePhysics(0.65f, 0.0f, 0.0f, DeltaSeconds);
}

AIUPoliceAIController::AIUPoliceAIController() = default;

void AIUPoliceAIController::EvaluateSuspectPursuit(AIUCharacter* Suspect, AIUPlayerState* SuspectPS, float MaxSightRangeCm)
{
    if (!Suspect || !SuspectPS || SuspectPS->WantedLevel <= 0)
    {
        TacticState = EIUPoliceTacticState::RoutinePatrol;
        bHasLineOfSightToSuspect = false;
        bSpikeStripDeployed = false;
        bHelicopterSpotlightLocked = false;
        return;
    }

    const float Dist = FVector::Dist(GetActorLocation(), Suspect->GetActorLocation());
    bHasLineOfSightToSuspect = (Dist <= MaxSightRangeCm);

    if (bHasLineOfSightToSuspect)
    {
        LastKnownSuspectLocation = Suspect->GetActorLocation();
    }

    switch (SuspectPS->WantedLevel)
    {
        case 1:
            TacticState = EIUPoliceTacticState::BeatOfficerFootPursuit;
            break;
        case 2:
            TacticState = EIUPoliceTacticState::InterceptorRammingPursuit;
            break;
        case 3:
            TacticState = EIUPoliceTacticState::RoadblockAndSpikeStrip;
            bSpikeStripDeployed = true;
            break;
        case 4:
            TacticState = EIUPoliceTacticState::TacticalSWATContainment;
            bSpikeStripDeployed = true;
            break;
        default:
            TacticState = EIUPoliceTacticState::ApexLockdownWithHeliSupport;
            bSpikeStripDeployed = true;
            bHelicopterSpotlightLocked = true;
            break;
    }
}

void AIUPoliceAIController::ExecutePursuitStep(AIUCharacter* PoliceOfficer, AIUCharacter* Suspect, float DeltaSeconds)
{
    if (!PoliceOfficer || !Suspect || DeltaSeconds <= 0.0f || TacticState == EIUPoliceTacticState::RoutinePatrol)
    {
        return;
    }

    const FVector ToTarget = (LastKnownSuspectLocation - PoliceOfficer->GetActorLocation()).GetSafeNormal();
    const float PursuitSpeed = (TacticState >= EIUPoliceTacticState::InterceptorRammingPursuit) ? 820.0f : 580.0f;
    PoliceOfficer->SetActorLocation(PoliceOfficer->GetActorLocation() + (ToTarget * PursuitSpeed * DeltaSeconds));
    SetActorLocation(PoliceOfficer->GetActorLocation());
}

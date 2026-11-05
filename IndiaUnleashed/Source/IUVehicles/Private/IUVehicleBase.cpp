// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.

#include "../Public/IUVehicleBase.h"
#include "../../IndiaUnleashed/Public/IUCharacter.h"

AIUVehicleBase::AIUVehicleBase()
{
    bReplicates = true;
    ConfigurePreset(EIUVehicleCategory::AutoRickshaw, "VH_HYD_AUTORICKSHAW_01");
}

void AIUVehicleBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    APawn::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AIUVehicleBase, VehicleModelId);
    DOREPLIFETIME(AIUVehicleBase, Category);
    DOREPLIFETIME(AIUVehicleBase, OwnerAccountId);
    DOREPLIFETIME(AIUVehicleBase, DriverCharacter);
    DOREPLIFETIME(AIUVehicleBase, MaxChassisHealth);
    DOREPLIFETIME(AIUVehicleBase, CurrentChassisHealth);
    DOREPLIFETIME(AIUVehicleBase, MaxFuelLiters);
    DOREPLIFETIME(AIUVehicleBase, CurrentFuelLiters);
    DOREPLIFETIME(AIUVehicleBase, CurrentSpeedKmH);
    DOREPLIFETIME(AIUVehicleBase, EngineRPM);
}

void AIUVehicleBase::ConfigurePreset(EIUVehicleCategory InCategory, const FString& InModelId)
{
    Category = InCategory;
    VehicleModelId = InModelId;

    switch (InCategory)
    {
        case EIUVehicleCategory::AutoRickshaw:
            TopSpeedKmH = 68.0f;
            AccelerationKmhPerSec = 18.0f;
            MaxFuelLiters = 12.0f;
            MaxChassisHealth = 650.0f;
            break;
        case EIUVehicleCategory::SedanCar:
            TopSpeedKmH = 185.0f;
            AccelerationKmhPerSec = 34.0f;
            MaxFuelLiters = 50.0f;
            MaxChassisHealth = 1200.0f;
            break;
        case EIUVehicleCategory::SportMotorcycle:
            TopSpeedKmH = 210.0f;
            AccelerationKmhPerSec = 48.0f;
            MaxFuelLiters = 16.0f;
            MaxChassisHealth = 550.0f;
            break;
        case EIUVehicleCategory::CityScooter:
            TopSpeedKmH = 82.0f;
            AccelerationKmhPerSec = 22.0f;
            MaxFuelLiters = 6.5f;
            MaxChassisHealth = 420.0f;
            break;
        case EIUVehicleCategory::CityBus:
            TopSpeedKmH = 105.0f;
            AccelerationKmhPerSec = 14.0f;
            MaxFuelLiters = 180.0f;
            MaxChassisHealth = 2800.0f;
            break;
        case EIUVehicleCategory::CargoTruck:
            TopSpeedKmH = 110.0f;
            AccelerationKmhPerSec = 12.0f;
            MaxFuelLiters = 240.0f;
            MaxChassisHealth = 3400.0f;
            break;
        case EIUVehicleCategory::MotorBoat:
            TopSpeedKmH = 95.0f;
            AccelerationKmhPerSec = 25.0f;
            MaxFuelLiters = 85.0f;
            MaxChassisHealth = 1100.0f;
            break;
        case EIUVehicleCategory::Helicopter:
            TopSpeedKmH = 240.0f;
            AccelerationKmhPerSec = 38.0f;
            MaxFuelLiters = 320.0f;
            MaxChassisHealth = 1600.0f;
            break;
    }
    CurrentChassisHealth = MaxChassisHealth;
    CurrentFuelLiters = MaxFuelLiters;
}

bool AIUVehicleBase::ServerAttemptPossessDriver(AIUCharacter* CandidateDriver)
{
    if (!HasAuthority() || CandidateDriver == nullptr || DriverCharacter != nullptr || IsDestroyed())
    {
        return false;
    }
    const float Dist = FVector::Dist(GetActorLocation(), CandidateDriver->GetActorLocation());
    if (Dist > 380.0f) // Must be within 3.8m of vehicle door/handlebar
    {
        return false;
    }

    DriverCharacter = CandidateDriver;
    CandidateDriver->SetActorLocation(GetActorLocation());
    return true;
}

bool AIUVehicleBase::ServerReleaseDriver(AIUCharacter* ExitingDriver)
{
    if (!HasAuthority() || ExitingDriver == nullptr || DriverCharacter != ExitingDriver)
    {
        return false;
    }
    DriverCharacter = nullptr;
    // Place exiting driver 180cm to the left of the vehicle
    FVector ExitLoc = GetActorLocation() + FVector(0.0f, -180.0f, 20.0f);
    ExitingDriver->SetActorLocation(ExitLoc);
    return true;
}

void AIUVehicleBase::SimulateVehiclePhysics(float ThrottleInput, float BrakeInput, float SteerInput, float DeltaSeconds)
{
    if (DeltaSeconds <= 0.0f || IsDestroyed())
    {
        CurrentSpeedKmH = 0.0f;
        EngineRPM = 0.0f;
        return;
    }

    const float ClampedThrottle = FMath::Clamp(ThrottleInput, 0.0f, 1.0f);
    const float ClampedBrake = FMath::Clamp(BrakeInput, 0.0f, 1.0f);
    const float ClampedSteer = FMath::Clamp(SteerInput, -1.0f, 1.0f);

    if (CurrentFuelLiters > 0.0f && ClampedThrottle > 0.0f && DriverCharacter != nullptr)
    {
        CurrentSpeedKmH = FMath::Min(TopSpeedKmH, CurrentSpeedKmH + (AccelerationKmhPerSec * ClampedThrottle * DeltaSeconds));
        CurrentFuelLiters = FMath::Max(0.0f, CurrentFuelLiters - (0.018f * ClampedThrottle * DeltaSeconds));
    }
    else
    {
        // Rolling resistance
        CurrentSpeedKmH = FMath::Max(0.0f, CurrentSpeedKmH - (6.0f * DeltaSeconds));
    }

    if (ClampedBrake > 0.0f)
    {
        CurrentSpeedKmH = FMath::Max(0.0f, CurrentSpeedKmH - (BrakingDecelerationKmhPerSec * ClampedBrake * DeltaSeconds));
    }

    EngineRPM = 850.0f + (CurrentSpeedKmH / FMath::Max(1.0f, TopSpeedKmH)) * 5600.0f;

    // Convert Km/H to cm/s (1 km/h = 27.7778 cm/s)
    const float SpeedCmPerSec = CurrentSpeedKmH * 27.7778f;
    if (SpeedCmPerSec > 1.0f)
    {
        FRotator NewRot = GetActorRotation();
        NewRot.Yaw += ClampedSteer * 65.0f * DeltaSeconds;
        SetActorRotation(NewRot);

        const FVector NewPos = GetActorLocation() + (GetActorForwardVector() * SpeedCmPerSec * DeltaSeconds);
        SetActorLocation(NewPos);
        if (DriverCharacter)
        {
            DriverCharacter->SetActorLocation(NewPos);
        }
    }
}

void AIUVehicleBase::ServerApplyVehicleDamage(float DamageAmount)
{
    if (!HasAuthority() || DamageAmount <= 0.0f)
    {
        return;
    }
    CurrentChassisHealth = FMath::Max(0.0f, CurrentChassisHealth - DamageAmount);
    if (CurrentChassisHealth <= 0.0f && DriverCharacter != nullptr)
    {
        // Eject and apply explosion damage to occupant
        AIUCharacter* Occupant = DriverCharacter;
        ServerReleaseDriver(Occupant);
        Occupant->ServerApplyDamage(65.0f, false, this);
    }
}

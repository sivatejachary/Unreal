// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.
// Server-Authoritative Modular Vehicle Base supporting Indian Vehicles:
// Sedan Cars, Motorcycles, Scooters, Hyderabad Auto-Rickshaws, TSRTC-inspired Buses,
// Cargo Trucks, Hussain Sagar Boats, and Police Interceptors.

#pragma once

#include "../../IndiaUnleashed/Public/IUUnrealCoreCompat.h"

class AIUCharacter;

UENUM(BlueprintType)
enum class EIUVehicleCategory : uint8
{
    SedanCar = 0,
    SportMotorcycle = 1,
    CityScooter = 2,
    AutoRickshaw = 3,
    CityBus = 4,
    CargoTruck = 5,
    MotorBoat = 6,
    Helicopter = 7
};

UCLASS()
class IUVEHICLES_API AIUVehicleBase : public APawn
{
    GENERATED_BODY()

public:
    AIUVehicleBase();

    UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category="IU|Vehicle")
    FString VehicleModelId = "VH_HYD_AUTORICKSHAW_01";

    UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category="IU|Vehicle")
    EIUVehicleCategory Category = EIUVehicleCategory::AutoRickshaw;

    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="IU|Vehicle")
    FString OwnerAccountId;

    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="IU|Vehicle")
    AIUCharacter* DriverCharacter = nullptr;

    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="IU|Vehicle")
    float MaxChassisHealth = 1000.0f;

    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="IU|Vehicle")
    float CurrentChassisHealth = 1000.0f;

    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="IU|Vehicle")
    float MaxFuelLiters = 45.0f;

    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="IU|Vehicle")
    float CurrentFuelLiters = 45.0f;

    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="IU|Vehicle")
    float CurrentSpeedKmH = 0.0f;

    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="IU|Vehicle")
    float EngineRPM = 850.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="IU|Vehicle")
    float TopSpeedKmH = 140.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="IU|Vehicle")
    float AccelerationKmhPerSec = 28.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="IU|Vehicle")
    float BrakingDecelerationKmhPerSec = 55.0f;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    void ConfigurePreset(EIUVehicleCategory InCategory, const FString& InModelId);
    bool ServerAttemptPossessDriver(AIUCharacter* CandidateDriver);
    bool ServerReleaseDriver(AIUCharacter* ExitingDriver);
    void SimulateVehiclePhysics(float ThrottleInput, float BrakeInput, float SteerInput, float DeltaSeconds);
    void ServerApplyVehicleDamage(float DamageAmount);
    bool IsDestroyed() const { return CurrentChassisHealth <= 0.0f; }
};

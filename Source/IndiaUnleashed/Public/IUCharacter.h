// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.
// Core Third-Person Playable Character for INDIA UNLEASHED.
// Supports Enhanced Input actions, Health/Stamina/Armor attributes, Sprinting,
// Crouching, Jumping, Swimming, World Interaction, Vehicle Possession, and Server RPCs.

#pragma once

#include "IUUnrealCoreCompat.h"
#include "IUCharacterMovementComponent.h"
#include "IUPlayerState.h"
#include "IUPlayerController.h"

class AIUVehicleBase;
class AIUWeaponBase;

USTRUCT(BlueprintType)
struct FIUCharacterAttributes
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Attributes")
    float MaxHealth = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Attributes")
    float CurrentHealth = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Attributes")
    float MaxStamina = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Attributes")
    float CurrentStamina = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Attributes")
    float MaxArmor = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Attributes")
    float CurrentArmor = 50.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Attributes")
    float StaminaDrainPerSecond = 14.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Attributes")
    float StaminaRegenPerSecond = 18.0f;
};

UCLASS()
class INDIAUNLEASHED_API AIUCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AIUCharacter();
    virtual ~AIUCharacter();

    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="IU|Character")
    FString ActiveCharacterId = TEXT("CH_001_AAA_FEMALE");

    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="IU|Character")
    FIUCharacterAttributes Attributes;

    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="IU|Character")
    bool bIsSprinting = false;

    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="IU|Character")
    bool bIsAiming = false;

    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="IU|Character")
    bool bIsDead = false;

    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="IU|Character")
    AIUVehicleBase* CurrentVehicle = nullptr;

    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="IU|Character")
    AIUWeaponBase* EquippedWeapon = nullptr;

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UIUCharacterMovementComponent* GetIUMovementComponent() const { return IUMovement; }

    // Enhanced Input Handlers
    void Input_Move(const FInputActionValue& Value);
    void Input_Look(const FInputActionValue& Value);
    void Input_StartSprint();
    void Input_StopSprint();
    void Input_ToggleCrouch();
    void Input_Jump();
    void Input_StartAim();
    void Input_StopAim();
    bool Input_Interact(AActor* TargetInteractable);

    // Server-Authoritative Combat & Health
    float ServerApplyDamage(float RawDamage, bool bIsHeadshot, AActor* DamageCauser, float MaxValidDistance = 25000.0f);
    void ServerHeal(float HealAmount);
    void ServerRestoreArmor(float ArmorAmount);
    void ServerRespawnAtTransform(const FTransform& SpawnTransform);

    // Vehicle Possession Routing
    bool ServerEnterVehicle(AIUVehicleBase* VehicleToEnter);
    bool ServerExitVehicle();

private:
    UIUCharacterMovementComponent* IUMovement = nullptr;
    FVector PendingMovementInput{0.0f, 0.0f, 0.0f};
};

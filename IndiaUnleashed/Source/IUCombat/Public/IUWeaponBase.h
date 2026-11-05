// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.
// Server-Authoritative Weapon & Inventory System supporting Shooting, Reloading,
// Deterministic Recoil, Attachments, and Anti-Cheat Server Hit Validation.

#pragma once

#include "../../IndiaUnleashed/Public/IUUnrealCoreCompat.h"
#include "../../IndiaUnleashed/Public/IUCharacter.h"

USTRUCT(BlueprintType)
struct FIUWeaponConfig
{
    GENERATED_BODY()

    FString WeaponId = "WPN_AR_INSAS_M1";
    FString DisplayName = "Vajra-556 Assault Rifle";
    float BaseDamage = 32.0f;
    float MaxEffectiveRangeCm = 18000.0f; // 180 meters
    int32 MagazineCapacity = 30;
    float FireIntervalSeconds = 0.10f; // 600 RPM
    float VerticalRecoilPerShot = 0.65f;
    float HorizontalRecoilPerShot = 0.20f;
};

UCLASS()
class IUCOMBAT_API AIUWeaponBase : public AActor
{
    GENERATED_BODY()

public:
    AIUWeaponBase();

    UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category="IU|Weapon")
    FIUWeaponConfig Config;

    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="IU|Weapon")
    int32 CurrentAmmoInMag = 30;

    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="IU|Weapon")
    int32 ReserveAmmo = 120;

    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="IU|Weapon")
    bool bHasSuppressorAttachment = false;

    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="IU|Weapon")
    bool bHasScopeAttachment = false;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    bool ServerFireShot(AIUCharacter* Shooter, AIUCharacter* TargetCharacter, bool bHeadshotClaimed, float CurrentWorldTimeSec, float& OutAppliedDamage);
    bool ServerReload();
    FRotator ComputeRecoilOffset(int32 ConsecutiveShotIndex) const;

private:
    float LastFireTimestampSec = -100.0f;
};

UCLASS(ClassGroup=(IndiaUnleashed), meta=(BlueprintSpawnableComponent))
class IUCOMBAT_API UIUInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UIUInventoryComponent();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IU|Inventory")
    int32 MaxWeaponSlots = 3;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IU|Inventory")
    float MaxCarryWeightKg = 35.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IU|Inventory")
    float CurrentCarryWeightKg = 0.0f;

    bool ServerAddWeapon(AIUWeaponBase* NewWeapon, float WeaponWeightKg = 3.8f);
    bool ServerSwitchWeaponSlot(int32 SlotIndex, AIUCharacter* OwnerCharacter);
    bool HasWeaponId(const FString& WeaponId) const;
    int32 GetWeaponCount() const { return static_cast<int32>(Weapons.size()); }

private:
    TArray<AIUWeaponBase*> Weapons;
    int32 ActiveSlotIndex = -1;
};

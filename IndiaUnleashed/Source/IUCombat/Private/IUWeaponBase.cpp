// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.

#include "../Public/IUWeaponBase.h"

AIUWeaponBase::AIUWeaponBase()
{
    bReplicates = true;
    CurrentAmmoInMag = Config.MagazineCapacity;
}

void AIUWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    AActor::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AIUWeaponBase, Config);
    DOREPLIFETIME(AIUWeaponBase, CurrentAmmoInMag);
    DOREPLIFETIME(AIUWeaponBase, ReserveAmmo);
    DOREPLIFETIME(AIUWeaponBase, bHasSuppressorAttachment);
    DOREPLIFETIME(AIUWeaponBase, bHasScopeAttachment);
}

bool AIUWeaponBase::ServerFireShot(AIUCharacter* Shooter, AIUCharacter* TargetCharacter, bool bHeadshotClaimed, float CurrentWorldTimeSec, float& OutAppliedDamage)
{
    OutAppliedDamage = 0.0f;
    if (!HasAuthority() || Shooter == nullptr || Shooter->bIsDead)
    {
        return false;
    }

    // 1. Validate ammo availability
    if (CurrentAmmoInMag <= 0)
    {
        return false;
    }

    // 2. Validate fire-rate cadence (allow 15% network jitter tolerance)
    if ((CurrentWorldTimeSec - LastFireTimestampSec) < (Config.FireIntervalSeconds * 0.85f))
    {
        return false; // Rapid-fire exploit rejected
    }

    CurrentAmmoInMag -= 1;
    LastFireTimestampSec = CurrentWorldTimeSec;

    // 3. If a target was hit, validate distance and apply authoritative damage
    if (TargetCharacter != nullptr && TargetCharacter != Shooter)
    {
        const float HitDist = FVector::Dist(Shooter->GetActorLocation(), TargetCharacter->GetActorLocation());
        if (HitDist > Config.MaxEffectiveRangeCm)
        {
            return false; // Out-of-range hit packet rejected
        }

        // Distance damage falloff beyond 50% of max effective range
        float FalloffScale = 1.0f;
        const float HalfRange = Config.MaxEffectiveRangeCm * 0.5f;
        if (HitDist > HalfRange)
        {
            FalloffScale = FMath::Clamp(1.0f - ((HitDist - HalfRange) / HalfRange) * 0.45f, 0.55f, 1.0f);
        }

        const float FinalRawDamage = Config.BaseDamage * FalloffScale;
        OutAppliedDamage = TargetCharacter->ServerApplyDamage(FinalRawDamage, bHeadshotClaimed, Shooter, Config.MaxEffectiveRangeCm);
    }

    return true;
}

bool AIUWeaponBase::ServerReload()
{
    if (!HasAuthority() || ReserveAmmo <= 0 || CurrentAmmoInMag >= Config.MagazineCapacity)
    {
        return false;
    }
    const int32 Needed = Config.MagazineCapacity - CurrentAmmoInMag;
    const int32 ToLoad = (ReserveAmmo < Needed) ? ReserveAmmo : Needed;
    CurrentAmmoInMag += ToLoad;
    ReserveAmmo -= ToLoad;
    return true;
}

FRotator AIUWeaponBase::ComputeRecoilOffset(int32 ConsecutiveShotIndex) const
{
    const float ClampedIdx = static_cast<float>(FMath::Clamp(ConsecutiveShotIndex, 1, 30));
    const float AttachmentScale = bHasSuppressorAttachment ? 0.85f : 1.0f;
    const float PitchKick = Config.VerticalRecoilPerShot * ClampedIdx * AttachmentScale;
    const float YawKick = ((ConsecutiveShotIndex % 2 == 0) ? 1.0f : -1.0f) * Config.HorizontalRecoilPerShot * AttachmentScale;
    return FRotator(PitchKick, YawKick, 0.0f);
}

UIUInventoryComponent::UIUInventoryComponent()
{
    SetIsReplicatedByDefault(true);
}

bool UIUInventoryComponent::ServerAddWeapon(AIUWeaponBase* NewWeapon, float WeaponWeightKg)
{
    if (NewWeapon == nullptr || WeaponWeightKg <= 0.0f)
    {
        return false;
    }
    if (static_cast<int32>(Weapons.size()) >= MaxWeaponSlots)
    {
        return false; // Slot limit reached
    }
    if ((CurrentCarryWeightKg + WeaponWeightKg) > MaxCarryWeightKg)
    {
        return false; // Weight limit exceeded
    }
    if (HasWeaponId(NewWeapon->Config.WeaponId))
    {
        return false; // Duplicate weapon rejected
    }

    Weapons.push_back(NewWeapon);
    CurrentCarryWeightKg += WeaponWeightKg;
    if (ActiveSlotIndex < 0)
    {
        ActiveSlotIndex = 0;
    }
    return true;
}

bool UIUInventoryComponent::ServerSwitchWeaponSlot(int32 SlotIndex, AIUCharacter* OwnerCharacter)
{
    if (SlotIndex < 0 || SlotIndex >= static_cast<int32>(Weapons.size()) || OwnerCharacter == nullptr)
    {
        return false;
    }
    ActiveSlotIndex = SlotIndex;
    OwnerCharacter->EquippedWeapon = Weapons[static_cast<size_t>(SlotIndex)];
    return true;
}

bool UIUInventoryComponent::HasWeaponId(const FString& WeaponId) const
{
    for (const auto* W : Weapons)
    {
        if (W && W->Config.WeaponId == WeaponId)
        {
            return true;
        }
    }
    return false;
}

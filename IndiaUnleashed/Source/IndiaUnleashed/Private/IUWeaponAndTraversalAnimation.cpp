// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.

#include "../Public/IUWeaponAndTraversalAnimation.h"

UIUAdvancedAnimationAndWeaponSubsystem::UIUAdvancedAnimationAndWeaponSubsystem()
{
    // Register all 50 AAA Animation Sequences
    for (int i = 1; i <= 50; ++i)
    {
        EIUAnimationSequenceId Id = static_cast<EIUAnimationSequenceId>(i);
        FIUAnimationMetadata Meta;
        Meta.SequenceId = Id;
        Meta.DurationSeconds = 1.2f;

        if (i <= 14) Meta.Group = EIUAnimationGroup::Locomotion;
        else if (i <= 24) Meta.Group = EIUAnimationGroup::Traversal;
        else if (i <= 36) Meta.Group = EIUAnimationGroup::Combat;
        else if (i <= 43) Meta.Group = EIUAnimationGroup::Reactions;
        else Meta.Group = EIUAnimationGroup::Interactions;

        if (i == 22 || i == 23 || i == 24)
        {
            Meta.bUsesRootMotion = true;
        }

        Master50AnimationLibrary[Id] = Meta;
    }

    // Register 5 Weapon Class Profiles
    WeaponClassProfiles[EIUGunClass::AssaultRifle] = {EIUGunClass::AssaultRifle, "AssaultRifle", 38.0f, 0.22f, 2.1f, 2.8f};
    WeaponClassProfiles[EIUGunClass::SMG]          = {EIUGunClass::SMG, "SMG",                   26.0f, 0.16f, 1.8f, 2.4f};
    WeaponClassProfiles[EIUGunClass::SniperRifle]  = {EIUGunClass::SniperRifle, "SniperRifle",   54.0f, 0.45f, 2.7f, 3.8f};
    WeaponClassProfiles[EIUGunClass::Pistol]       = {EIUGunClass::Pistol, "Pistol",             18.0f, 0.18f, 1.4f, 1.9f};
    WeaponClassProfiles[EIUGunClass::Shotgun]      = {EIUGunClass::Shotgun, "Shotgun",           32.0f, 0.38f, 2.6f, 3.4f};
}

int32 UIUAdvancedAnimationAndWeaponSubsystem::GetSequenceCountByGroup(EIUAnimationGroup Group) const
{
    int32 Count = 0;
    for (const auto& Pair : Master50AnimationLibrary)
    {
        if (Pair.second.Group == Group) ++Count;
    }
    return Count;
}

const FIUAnimationMetadata* UIUAdvancedAnimationAndWeaponSubsystem::GetAnimationMetadata(EIUAnimationSequenceId SeqId) const
{
    auto It = Master50AnimationLibrary.find(SeqId);
    if (It != Master50AnimationLibrary.end())
    {
        return &It->second;
    }
    return nullptr;
}

EIUAnimationSequenceId UIUAdvancedAnimationAndWeaponSubsystem::SelectReloadAnimation(EIUGunClass InGunClass, int32 RemainingMagAmmo, float& OutDurationSec)
{
    const auto& Prof = WeaponClassProfiles[InGunClass];
    if (RemainingMagAmmo > 0)
    {
        OutDurationSec = Prof.TacticalReloadDurationSec;
        return EIUAnimationSequenceId::Anim31_Reload_Tactical_MagSwap;
    }
    else
    {
        OutDurationSec = Prof.EmptyReloadDurationSec;
        return EIUAnimationSequenceId::Anim32_Reload_Empty_BoltRelease;
    }
}

void UIUAdvancedAnimationAndWeaponSubsystem::ApplyWeaponClassHandIKAndRecoil(
    EIUGunClass InGunClass, bool bInADS, bool bIsReloading, int32 RecoilKickIndex, FIUFullBodyIKState& OutIKState)
{
    (void)bInADS;
    (void)RecoilKickIndex;
    const auto& Prof = WeaponClassProfiles[InGunClass];
    if (bIsReloading)
    {
        OutIKState.LeftHandIKAlpha = 0.0f;
    }
    else
    {
        OutIKState.LeftHandIKAlpha = 1.0f;
        OutIKState.LeftHandForegripLocalOffset = FVector(0.0f, Prof.ForegripOffsetCm, 0.0f);
    }
}

bool UIUAdvancedAnimationAndWeaponSubsystem::TriggerObstacleVaultOrClimb(float ObstacleHeightCm, float ApproachSpeedCmPerSec)
{
    VaultState.bIsTraversing = true;
    VaultState.Progress01 = 0.0f;
    VaultState.ObstacleHeightCm = ObstacleHeightCm;

    if (ObstacleHeightCm > 180.0f)
    {
        VaultState.ActiveTraversalSeq = EIUAnimationSequenceId::Anim23_Climb_Ledge_210cm;
    }
    else if (ApproachSpeedCmPerSec > 300.0f)
    {
        VaultState.ActiveTraversalSeq = EIUAnimationSequenceId::Anim24_Obstacle_Window_Traversal;
    }
    else
    {
        VaultState.ActiveTraversalSeq = EIUAnimationSequenceId::Anim22_Vault_LowBarrier_110cm;
    }

    return true;
}

void UIUAdvancedAnimationAndWeaponSubsystem::StepCascadeurBallisticTraversal(float DeltaTime)
{
    if (!VaultState.bIsTraversing) return;
    VaultState.Progress01 += DeltaTime;

    if (VaultState.Progress01 >= 1.0f)
    {
        VaultState.bIsTraversing = false;
        VaultState.Progress01 = 1.0f;
        VaultState.PelvisClearanceOffsetZCm = 0.0f;
        VaultState.LeftHandPlantIKAlpha = 0.0f;
        return;
    }

    // Parabolic arc for center-of-mass clearance
    float SinT = FMath::Sin(VaultState.Progress01 * 3.14159f);
    VaultState.PelvisClearanceOffsetZCm = VaultState.ObstacleHeightCm * (0.85f + 0.20f * SinT);

    // Support hand IK locked onto barrier crest near midpoint
    if (VaultState.Progress01 >= 0.25f && VaultState.Progress01 <= 0.75f)
    {
        VaultState.LeftHandPlantIKAlpha = 1.0f;
    }
    else
    {
        VaultState.LeftHandPlantIKAlpha = 0.0f;
    }
}

void UIUAdvancedAnimationAndWeaponSubsystem::EvaluateMobileAnimationLOD(
    float DistanceToCameraMeters, int32& OutTickRateHz, int32& OutEvaluatedBones, bool& bOutEvaluateFingers)
{
    if (DistanceToCameraMeters < 15.0f)
    {
        OutTickRateHz = 60;
        OutEvaluatedBones = 56;
        bOutEvaluateFingers = true;
    }
    else if (DistanceToCameraMeters < 40.0f)
    {
        OutTickRateHz = 30;
        OutEvaluatedBones = 38;
        bOutEvaluateFingers = false;
    }
    else
    {
        OutTickRateHz = 15;
        OutEvaluatedBones = 22;
        bOutEvaluateFingers = false;
    }
}

void UIUAdvancedAnimationAndWeaponSubsystem::ApplyCharacterAppearanceVariation(const FString& OutfitId, FString& OutMasterMeshPath)
{
    (void)OutfitId;
    OutMasterMeshPath = "/Game/IndiaUnleashed/Characters/Heroes/SK_IU_Humanoid_Master.uasset";
}

FIUReplicatedAnimationState UIUAdvancedAnimationAndWeaponSubsystem::BuildReplicatedAnimationPacket(
    EIUAnimationSequenceId LocomotionSeq, EIUAnimationSequenceId UpperBodySeq,
    float SpeedCmPerSec, float AimPitchDeg, float LeanDeg,
    bool bDBNO, bool bFiring, bool bADS, bool bReloadingEmpty, bool bVaulting)
{
    (void)AimPitchDeg;
    FIUReplicatedAnimationState Pkt;
    Pkt.ActiveLocomotionSeq = LocomotionSeq;
    Pkt.ActiveUpperBodySeq = UpperBodySeq;
    Pkt.EquippedGunClass = CurrentWeaponClass;
    Pkt.QuantizedSpeedCmPerSec = (int16)SpeedCmPerSec;
    Pkt.QuantizedLeanAngleDeg = (int8)LeanDeg;
    Pkt.bIsVaultingOrClimbing = bVaulting;
    Pkt.bIsReloadingEmpty = bReloadingEmpty;
    Pkt.bIsADS = bADS;
    Pkt.bIsDBNO = bDBNO;
    Pkt.bIsFiring = bFiring;

    return Pkt;
}

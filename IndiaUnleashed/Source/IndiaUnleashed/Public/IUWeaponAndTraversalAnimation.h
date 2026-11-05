// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.
// Complete 50-Sequence AAA Animation Library, 5 Weapon Classes Hand IK,
// Cascadeur Ballistic Traversal Physics, Mobile URO & Bone LOD Optimization.

#pragma once

#include "IUUnrealCoreCompat.h"
#include "IUAnimationSystem.h"

UENUM(BlueprintType)
enum class EIUAnimationGroup : uint8
{
    Locomotion = 0,
    Traversal = 1,
    Combat = 2,
    Reactions = 3,
    Interactions = 4
};

UENUM(BlueprintType)
enum class EIUAnimationSequenceId : uint8
{
    Anim01_Idle_Breathing = 1,
    Anim02_Walk_Forward = 2,
    Anim03_Walk_Backward = 3,
    Anim04_Walk_Left_Strafe = 4,
    Anim05_Walk_Right_Strafe = 5,
    Anim06_Jog_Forward = 6,
    Anim07_Jog_Forward_Left = 7,
    Anim08_Jog_Forward_Right = 8,
    Anim09_Sprint_FullSpeed = 9,
    Anim10_Crouch_Idle = 10,
    Anim11_Crouch_Walk_Forward = 11,
    Anim12_Crouch_Walk_Strafe = 12,
    Anim13_Prone_Idle = 13,
    Anim14_Prone_Crawl_Forward = 14,
    Anim15_Jump_Start_Takeoff = 15,
    Anim16_Jump_Apex_AirLoop = 16,
    Anim17_Jump_Land_Light = 17,
    Anim18_Jump_Land_HeavyRoll = 18,
    Anim19_Swim_Breaststroke = 19,
    Anim20_Swim_TreadWater = 20,
    Anim21_Slide_TacticalCrouch = 21,
    Anim22_Vault_LowBarrier_110cm = 22,
    Anim23_Climb_Ledge_210cm = 23,
    Anim24_Obstacle_Window_Traversal = 24,
    Anim25_Weapon_Equip_Draw = 25,
    Anim26_Weapon_Ready_Hold = 26,
    Anim27_Hip_Fire_Burst = 27,
    Anim28_ADS_Sight_Align = 28,
    Anim29_ADS_Precision_Fire = 29,
    Anim30_Recoil_Spring_Recovery = 30,
    Anim31_Reload_Tactical_MagSwap = 31,
    Anim32_Reload_Empty_BoltRelease = 32,
    Anim33_Weapon_Switch_Holster = 33,
    Anim34_Aim_Walk_8Way_Blend = 34,
    Anim35_Crouched_ADS_Fire = 35,
    Anim36_Prone_Supported_Fire = 36,
    Anim37_Hit_Torso_Flinch = 37,
    Anim38_Hit_Head_Knockback = 38,
    Anim39_Hit_Leg_Limp = 39,
    Anim40_Knocked_Down_DBNO = 40,
    Anim41_Revive_Ally_Bandage = 41,
    Anim42_Death_Ragdoll_Collapse = 42,
    Anim43_Death_Blast_Impulse = 43,
    Anim44_Open_Door_Handle = 44,
    Anim45_Loot_Pickup_Ground = 45,
    Anim46_Consume_EnergyDrink = 46,
    Anim47_Apply_FirstAidMedkit = 47,
    Anim48_Enter_Vehicle_Driver = 48,
    Anim49_Exit_Vehicle_Dismount = 49,
    Anim50_Vehicle_Steering_Wheel = 50
};

UENUM(BlueprintType)
enum class EIUGunClass : uint8
{
    AssaultRifle = 0,
    SMG = 1,
    SniperRifle = 2,
    Pistol = 3,
    Shotgun = 4
};

USTRUCT(BlueprintType)
struct FIUAnimationMetadata
{
    GENERATED_BODY()

    EIUAnimationSequenceId SequenceId;
    FString CodeName;
    EIUAnimationGroup Group;
    float DurationSeconds = 1.0f;
    bool bUsesRootMotion = false;
    bool bCanLoop = false;
};

USTRUCT(BlueprintType)
struct FIUWeaponClassProfile
{
    GENERATED_BODY()

    EIUGunClass GunClass;
    FString ClassName;
    float ForegripOffsetCm = 35.0f;
    float RecoilRecoveryTimeSec = 0.22f;
    float TacticalReloadDurationSec = 2.1f;
    float EmptyReloadDurationSec = 3.0f;
};

USTRUCT(BlueprintType)
struct FIUVaultPhysicsState
{
    GENERATED_BODY()

    bool bIsTraversing = false;
    float Progress01 = 0.0f;
    float ObstacleHeightCm = 110.0f;
    float PelvisClearanceOffsetZCm = 0.0f;
    float LeftHandPlantIKAlpha = 0.0f;
    EIUAnimationSequenceId ActiveTraversalSeq;
};

USTRUCT(BlueprintType)
struct FIUReplicatedAnimationState
{
    GENERATED_BODY()

    EIUAnimationSequenceId ActiveLocomotionSeq;
    EIUAnimationSequenceId ActiveUpperBodySeq;
    EIUGunClass EquippedGunClass;
    int16 QuantizedSpeedCmPerSec;
    int8 QuantizedLeanAngleDeg;
    bool bIsVaultingOrClimbing;
    bool bIsReloadingEmpty;
    bool bIsADS;
    bool bIsDBNO;
    bool bIsFiring;
};

UCLASS()
class INDIAUNLEASHED_API UIUAdvancedAnimationAndWeaponSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UIUAdvancedAnimationAndWeaponSubsystem();
    virtual ~UIUAdvancedAnimationAndWeaponSubsystem() = default;

    std::map<EIUAnimationSequenceId, FIUAnimationMetadata> Master50AnimationLibrary;
    std::map<EIUGunClass, FIUWeaponClassProfile> WeaponClassProfiles;

    EIUGunClass CurrentWeaponClass = EIUGunClass::AssaultRifle;
    FIUVaultPhysicsState VaultState;

    int32 GetSequenceCountByGroup(EIUAnimationGroup Group) const;
    const FIUAnimationMetadata* GetAnimationMetadata(EIUAnimationSequenceId SeqId) const;

    EIUAnimationSequenceId SelectReloadAnimation(EIUGunClass InGunClass, int32 RemainingMagAmmo, float& OutDurationSec);
    void ApplyWeaponClassHandIKAndRecoil(EIUGunClass InGunClass, bool bInADS, bool bIsReloading, int32 RecoilKickIndex, FIUFullBodyIKState& OutIKState);

    bool TriggerObstacleVaultOrClimb(float ObstacleHeightCm, float ApproachSpeedCmPerSec);
    void StepCascadeurBallisticTraversal(float DeltaTime);

    void EvaluateMobileAnimationLOD(float DistanceToCameraMeters, int32& OutTickRateHz, int32& OutEvaluatedBones, bool& bOutEvaluateFingers);
    void ApplyCharacterAppearanceVariation(const FString& OutfitId, FString& OutMasterMeshPath);

    FIUReplicatedAnimationState BuildReplicatedAnimationPacket(
        EIUAnimationSequenceId LocomotionSeq, EIUAnimationSequenceId UpperBodySeq,
        float SpeedCmPerSec, float AimPitchDeg, float LeanDeg,
        bool bDBNO, bool bFiring, bool bADS, bool bReloadingEmpty, bool bVaulting);
};

// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.
// Step 3 Animation Recreation: Motion Matching Pose Search, Two-Bone Foot IK,
// Two-Bone Hand Weapon IK, Full-Body IK, Root Motion & Stride Warping (zero foot slide),
// Turn-in-Place, Jump/Land Spring Compression, Physical Hit Reactions,
// Facial Morph Targets, Saccadic Eye Tracking, and Audio Phoneme Lip-Synchronization.

#pragma once

#include "IUUnrealCoreCompat.h"
#include "IUCharacterMovementComponent.h"

USTRUCT(BlueprintType)
struct FIUMotionPoseCandidate
{
    GENERATED_BODY()

    FString PoseAnimationName;
    EIULocomotionGait Gait = EIULocomotionGait::Jog;
    float ReferenceSpeedCmPerSec = 500.0f;
    float ReferenceTurnRateDegPerSec = 0.0f;
    bool bUsesRootMotion = true;
};

USTRUCT(BlueprintType)
struct FIUFullBodyIKState
{
    GENERATED_BODY()

    // Foot IK (eliminates stair/slope clipping & foot sliding)
    float LeftFootIKOffsetZ = 0.0f;
    float RightFootIKOffsetZ = 0.0f;
    float PelvisIKOffsetZ = 0.0f;
    float LeftFootSlopePitchDeg = 0.0f;
    float RightFootSlopePitchDeg = 0.0f;

    // Hand IK (locks off-hand to rifle foregrip & right hand to recoil spring)
    FVector LeftHandForegripLocalOffset{-18.0f, 42.0f, 12.0f};
    float LeftHandIKAlpha = 1.0f;
    float RightHandRecoilKickCm = 0.0f;

    // Spine Full-Body Aim & Hit Reaction Offset
    FRotator SpineAimOffset{0.0f, 0.0f, 0.0f};
    FRotator HitReactionSpineImpulse{0.0f, 0.0f, 0.0f};
};

USTRUCT(BlueprintType)
struct FIUFacialAndLipSyncState
{
    GENERATED_BODY()

    // Eye Tracking (clamped to physiological ±32 deg Yaw, ±22 deg Pitch)
    FRotator LeftEyeLookAtRotation{0.0f, 0.0f, 0.0f};
    FRotator RightEyeLookAtRotation{0.0f, 0.0f, 0.0f};
    float BlinkMorphWeight = 0.0f;

    // Lip Synchronization Morph Target Weights [0.0 .. 1.0]
    float Morph_JawOpen = 0.0f;
    float Morph_MouthWide = 0.0f;
    float Morph_LipFunnel = 0.0f;
    float Morph_BrowRaise = 0.0f;
    FString ActivePhoneme = "SIL";
};

UCLASS(ClassGroup=(IndiaUnleashed), meta=(BlueprintSpawnableComponent))
class INDIAUNLEASHED_API UIUAnimationInstanceComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UIUAnimationInstanceComponent();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IU|Animation")
    TArray<FIUMotionPoseCandidate> PoseSearchDatabase;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IU|Animation")
    FString ActiveMatchedPoseName = "MM_Idle_Relaxed";

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IU|Animation")
    float StrideWarpingScale = 1.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IU|Animation")
    float RootYawOffsetDeg = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IU|Animation")
    bool bIsTurningInPlace = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IU|Animation")
    float LandingPelvisCompressionCm = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IU|Animation")
    FIUFullBodyIKState IKState;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IU|Animation")
    FIUFacialAndLipSyncState FacialState;

    void InitializePoseSearchDatabase();

    // 1. Motion Matching & Stride Warping (eliminates robotic transitions & sliding feet)
    FString EvaluateMotionMatchingPose(float CurrentSpeedCmPerSec, float TurnRateDegPerSec, EIULocomotionGait CurrentGait);

    // 2. Two-Bone Foot IK, Hand Weapon IK & Full-Body IK
    void SolveFullBodyIK(float ActorWorldZ, float LeftFootFloorZ, float RightFootFloorZ, float LeftFloorSlopeDeg, float RightFloorSlopeDeg, bool bWeaponEquipped, float DeltaSeconds);

    // 3. Turn-in-Place, Jump/Landing Compression & Physical Hit Reaction
    void UpdateTurnInPlaceAndLanding(float CameraYawDeltaDeg, float ImpactVelocityZ, float DeltaSeconds);
    void ApplyPhysicalHitReaction(const FVector& ShotDirectionNormal, float DamageAmount);

    // 4. Facial Animation, Eye Tracking & Lip Sync
    void UpdateEyeTrackingAndBlinks(const FVector& HeadWorldLocation, const FRotator& HeadWorldRotation, const FVector& LookAtTargetWorldLocation, float DeltaSeconds);
    void UpdateLipSyncFromAudio(float AudioRmsAmplitude, char PhonemeCode);

private:
    float BlinkTimerSec = 0.0f;
};

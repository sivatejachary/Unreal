// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.

#include "../Public/IUAnimationSystem.h"

UIUAnimationInstanceComponent::UIUAnimationInstanceComponent()
{
    InitializePoseSearchDatabase();
}

void UIUAnimationInstanceComponent::InitializePoseSearchDatabase()
{
    PoseSearchDatabase.clear();
    PoseSearchDatabase.push_back({"MM_Idle_Breathing",      EIULocomotionGait::Walk,        0.0f,   0.0f, true});
    PoseSearchDatabase.push_back({"MM_Walk_Natural_Start",  EIULocomotionGait::Walk,      240.0f,   0.0f, true});
    PoseSearchDatabase.push_back({"MM_Walk_TurnLeft45",     EIULocomotionGait::Walk,      220.0f, -45.0f, true});
    PoseSearchDatabase.push_back({"MM_Walk_TurnRight45",    EIULocomotionGait::Walk,      220.0f,  45.0f, true});
    PoseSearchDatabase.push_back({"MM_Jog_Tactical_Loop",   EIULocomotionGait::Jog,       500.0f,   0.0f, true});
    PoseSearchDatabase.push_back({"MM_Jog_LeanLeft",        EIULocomotionGait::Jog,       480.0f, -60.0f, true});
    PoseSearchDatabase.push_back({"MM_Jog_LeanRight",       EIULocomotionGait::Jog,       480.0f,  60.0f, true});
    PoseSearchDatabase.push_back({"MM_Sprint_FullDrive",    EIULocomotionGait::Sprint,    760.0f,   0.0f, true});
    PoseSearchDatabase.push_back({"MM_Crouch_TacticalStalk",EIULocomotionGait::Crouch,    260.0f,   0.0f, true});
    PoseSearchDatabase.push_back({"MM_Swim_Breaststroke",   EIULocomotionGait::SwimSurface,320.0f,  0.0f, true});
}

FString UIUAnimationInstanceComponent::EvaluateMotionMatchingPose(float CurrentSpeedCmPerSec, float TurnRateDegPerSec, EIULocomotionGait CurrentGait)
{
    if (CurrentSpeedCmPerSec < 8.0f && CurrentGait != EIULocomotionGait::Crouch && CurrentGait != EIULocomotionGait::SwimSurface)
    {
        ActiveMatchedPoseName = "MM_Idle_Breathing";
        StrideWarpingScale = 1.0f;
        return ActiveMatchedPoseName;
    }

    float BestCost = 1e30f;
    const FIUMotionPoseCandidate* BestPose = nullptr;

    for (const auto& Cand : PoseSearchDatabase)
    {
        const float GaitPenalty = (Cand.Gait == CurrentGait) ? 0.0f : 350.0f;
        const float SpeedCost = FMath::Abs(CurrentSpeedCmPerSec - Cand.ReferenceSpeedCmPerSec);
        const float TurnCost = FMath::Abs(TurnRateDegPerSec - Cand.ReferenceTurnRateDegPerSec) * 1.5f;
        const float TotalCost = GaitPenalty + SpeedCost + TurnCost;

        if (TotalCost < BestCost)
        {
            BestCost = TotalCost;
            BestPose = &Cand;
        }
    }

    if (BestPose)
    {
        ActiveMatchedPoseName = BestPose->PoseAnimationName;
        if (BestPose->ReferenceSpeedCmPerSec > 10.0f)
        {
            // Stride Warping locks foot speed to exact ground speed -> Zero Foot Sliding!
            StrideWarpingScale = FMath::Clamp(CurrentSpeedCmPerSec / BestPose->ReferenceSpeedCmPerSec, 0.65f, 1.45f);
        }
        else
        {
            StrideWarpingScale = 1.0f;
        }
    }

    return ActiveMatchedPoseName;
}

void UIUAnimationInstanceComponent::SolveFullBodyIK(
    float ActorWorldZ,
    float LeftFootFloorZ,
    float RightFootFloorZ,
    float LeftFloorSlopeDeg,
    float RightFloorSlopeDeg,
    bool bWeaponEquipped,
    float DeltaSeconds)
{
    if (DeltaSeconds <= 0.0f) return;

    const float TargetLeftDeltaZ = FMath::Clamp(LeftFootFloorZ - ActorWorldZ, -45.0f, 40.0f);
    const float TargetRightDeltaZ = FMath::Clamp(RightFootFloorZ - ActorWorldZ, -45.0f, 40.0f);

    IKState.LeftFootIKOffsetZ = FMath::FInterpTo(IKState.LeftFootIKOffsetZ, TargetLeftDeltaZ, DeltaSeconds, 16.0f);
    IKState.RightFootIKOffsetZ = FMath::FInterpTo(IKState.RightFootIKOffsetZ, TargetRightDeltaZ, DeltaSeconds, 16.0f);

    // Lower pelvis to match the lowest foot so neither leg hyper-extends on steps/slopes
    const float TargetPelvisZ = FMath::Min(IKState.LeftFootIKOffsetZ, IKState.RightFootIKOffsetZ) - LandingPelvisCompressionCm;
    IKState.PelvisIKOffsetZ = FMath::FInterpTo(IKState.PelvisIKOffsetZ, TargetPelvisZ, DeltaSeconds, 14.0f);

    IKState.LeftFootSlopePitchDeg = FMath::Clamp(LeftFloorSlopeDeg, -38.0f, 38.0f);
    IKState.RightFootSlopePitchDeg = FMath::Clamp(RightFloorSlopeDeg, -38.0f, 38.0f);

    // Hand Weapon IK Alpha
    IKState.LeftHandIKAlpha = bWeaponEquipped ? 1.0f : 0.0f;
    IKState.RightHandRecoilKickCm = FMath::FInterpTo(IKState.RightHandRecoilKickCm, 0.0f, DeltaSeconds, 18.0f);

    // Recover physical hit-reaction spine impulse smoothly back to zero
    IKState.HitReactionSpineImpulse.Pitch = FMath::FInterpTo(IKState.HitReactionSpineImpulse.Pitch, 0.0f, DeltaSeconds, 10.0f);
    IKState.HitReactionSpineImpulse.Roll  = FMath::FInterpTo(IKState.HitReactionSpineImpulse.Roll,  0.0f, DeltaSeconds, 10.0f);
}

void UIUAnimationInstanceComponent::UpdateTurnInPlaceAndLanding(float CameraYawDeltaDeg, float ImpactVelocityZ, float DeltaSeconds)
{
    RootYawOffsetDeg += CameraYawDeltaDeg;
    if (FMath::Abs(RootYawOffsetDeg) >= 45.0f)
    {
        bIsTurningInPlace = true;
    }
    if (bIsTurningInPlace)
    {
        RootYawOffsetDeg = FMath::FInterpTo(RootYawOffsetDeg, 0.0f, DeltaSeconds, 8.0f);
        if (FMath::Abs(RootYawOffsetDeg) < 2.0f)
        {
            RootYawOffsetDeg = 0.0f;
            bIsTurningInPlace = false;
        }
    }

    // Apply landing knee/pelvis spring compression proportional to downward impact speed
    if (ImpactVelocityZ < -200.0f)
    {
        const float NormalizedImpact = FMath::Clamp(FMath::Abs(ImpactVelocityZ) / 980.0f, 0.1f, 1.0f);
        LandingPelvisCompressionCm = NormalizedImpact * 26.0f;
    }
    else
    {
        LandingPelvisCompressionCm = FMath::FInterpTo(LandingPelvisCompressionCm, 0.0f, DeltaSeconds, 9.0f);
    }
}

void UIUAnimationInstanceComponent::ApplyPhysicalHitReaction(const FVector& ShotDirectionNormal, float DamageAmount)
{
    const float ImpulseScale = FMath::Clamp(DamageAmount / 35.0f, 0.35f, 1.5f);
    IKState.HitReactionSpineImpulse.Pitch = ShotDirectionNormal.X * 18.0f * ImpulseScale;
    IKState.HitReactionSpineImpulse.Roll  = ShotDirectionNormal.Y * 16.0f * ImpulseScale;
    IKState.RightHandRecoilKickCm = 4.5f * ImpulseScale;
}

void UIUAnimationInstanceComponent::UpdateEyeTrackingAndBlinks(
    const FVector& HeadWorldLocation,
    const FRotator& HeadWorldRotation,
    const FVector& LookAtTargetWorldLocation,
    float DeltaSeconds)
{
    const FVector ToTarget = (LookAtTargetWorldLocation - HeadWorldLocation).GetSafeNormal();
    const float TargetYawDeg = std::atan2(ToTarget.Y, ToTarget.X) * (180.0f / 3.1415926535f) - HeadWorldRotation.Yaw;
    const float TargetPitchDeg = std::asin(FMath::Clamp(ToTarget.Z, -1.0f, 1.0f)) * (180.0f / 3.1415926535f) - HeadWorldRotation.Pitch;

    const float ClampedYaw = FMath::Clamp(TargetYawDeg, -32.0f, 32.0f);
    const float ClampedPitch = FMath::Clamp(TargetPitchDeg, -22.0f, 22.0f);

    FacialState.LeftEyeLookAtRotation  = FRotator(ClampedPitch, ClampedYaw, 0.0f);
    FacialState.RightEyeLookAtRotation = FRotator(ClampedPitch, ClampedYaw, 0.0f);

    // Periodic natural blink every 3.4 seconds
    BlinkTimerSec += DeltaSeconds;
    if (BlinkTimerSec >= 3.4f)
    {
        FacialState.BlinkMorphWeight = 1.0f;
        if (BlinkTimerSec >= 3.55f)
        {
            FacialState.BlinkMorphWeight = 0.0f;
            BlinkTimerSec = 0.0f;
        }
    }
}

void UIUAnimationInstanceComponent::UpdateLipSyncFromAudio(float AudioRmsAmplitude, char PhonemeCode)
{
    const float ClampedAmp = FMath::Clamp(AudioRmsAmplitude, 0.0f, 1.0f);
    if (ClampedAmp <= 0.02f)
    {
        FacialState.Morph_JawOpen = 0.0f;
        FacialState.Morph_MouthWide = 0.0f;
        FacialState.Morph_LipFunnel = 0.0f;
        FacialState.ActivePhoneme = "SIL";
        return;
    }

    switch (PhonemeCode)
    {
        case 'A':
            FacialState.Morph_JawOpen = ClampedAmp * 0.90f;
            FacialState.Morph_MouthWide = ClampedAmp * 0.45f;
            FacialState.Morph_LipFunnel = 0.10f;
            FacialState.ActivePhoneme = "AA";
            break;
        case 'E':
        case 'I':
            FacialState.Morph_JawOpen = ClampedAmp * 0.45f;
            FacialState.Morph_MouthWide = ClampedAmp * 0.85f;
            FacialState.Morph_LipFunnel = 0.0f;
            FacialState.ActivePhoneme = "EE";
            break;
        case 'O':
        case 'U':
            FacialState.Morph_JawOpen = ClampedAmp * 0.65f;
            FacialState.Morph_MouthWide = 0.05f;
            FacialState.Morph_LipFunnel = ClampedAmp * 0.92f;
            FacialState.ActivePhoneme = "OH";
            break;
        default:
            FacialState.Morph_JawOpen = ClampedAmp * 0.35f;
            FacialState.Morph_MouthWide = ClampedAmp * 0.30f;
            FacialState.Morph_LipFunnel = ClampedAmp * 0.25f;
            FacialState.ActivePhoneme = "CONSONANT";
            break;
    }
}

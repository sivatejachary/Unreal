// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.

#include "../Public/IUMobileTouchInputSystem.h"

UIUMobileTouchInputSubsystem::UIUMobileTouchInputSubsystem()
{
    ScopeSensitivityScalers[(int)EIUScopeOpticType::IronSight] = 1.00f;
    ScopeSensitivityScalers[(int)EIUScopeOpticType::RedDot]    = 0.90f;
    ScopeSensitivityScalers[(int)EIUScopeOpticType::Holo]      = 0.90f;
    ScopeSensitivityScalers[(int)EIUScopeOpticType::Scope2x]   = 0.70f;
    ScopeSensitivityScalers[(int)EIUScopeOpticType::Scope3x]   = 0.50f;
    ScopeSensitivityScalers[(int)EIUScopeOpticType::Scope4x]   = 0.35f;
    ScopeSensitivityScalers[(int)EIUScopeOpticType::Scope6x]   = 0.22f;
    ScopeSensitivityScalers[(int)EIUScopeOpticType::Scope8x]   = 0.14f;

    LoadLayoutPreset(EIUMobileLayoutPreset::TwoFingerThumb);
}

bool UIUMobileTouchInputSubsystem::OnTouchBegan(int32 FingerIndex, const FVector2D& ScreenNormalizedPos, const FString& BoundAction)
{
    if (FingerIndex < 0 || FingerIndex >= 10) return false;
    TouchSlots[FingerIndex].bActive = true;
    TouchSlots[FingerIndex].StartPos = ScreenNormalizedPos;
    TouchSlots[FingerIndex].CurrentPos = ScreenNormalizedPos;
    TouchSlots[FingerIndex].BoundAction = BoundAction;

    if (BoundAction == "BTN_FIRE_LEFT")
    {
        bLeftFirePressed = true;
    }
    else if (BoundAction == "BTN_FIRE_RIGHT")
    {
        bRightFirePressed = true;
    }
    else if (BoundAction == "BTN_ADS_SCOPE")
    {
        bADSPressed = true;
        if (ActiveOptic == EIUScopeOpticType::IronSight)
        {
            ActiveOptic = EIUScopeOpticType::Scope4x;
        }
    }

    return true;
}

void UIUMobileTouchInputSubsystem::OnTouchMoved(int32 FingerIndex, const FVector2D& ScreenNormalizedPos)
{
    if (FingerIndex < 0 || FingerIndex >= 10 || !TouchSlots[FingerIndex].bActive) return;
    TouchSlots[FingerIndex].CurrentPos = ScreenNormalizedPos;

    if (FingerIndex == 0)
    {
        // Left Joystick processing
        FVector2D Delta = ScreenNormalizedPos - JoystickCenter;
        float Dist = Delta.Size();
        if (Dist < DeadzoneRadius)
        {
            AnalogMoveVector = FVector2D::ZeroVector;
        }
        else
        {
            float Mag = (Dist - DeadzoneRadius) / (JoystickMaxTravelRadius - DeadzoneRadius + 0.001f);
            Mag = FMath::Clamp(Mag, 0.0f, 1.0f);
            FVector2D Dir = Delta / Dist;
            // Screen Y is down -> Invert for forward (Y > 0)
            AnalogMoveVector = FVector2D(Dir.X * Mag, -Dir.Y * Mag);

            // Check Sprint Lock drag (dragged far up)
            if (ScreenNormalizedPos.Y < (JoystickCenter.Y - 0.20f))
            {
                bSprintLocked = true;
            }
        }
    }
    else if (FingerIndex == 1)
    {
        // Right Camera rotation swipe
        FVector2D Delta = ScreenNormalizedPos - TouchSlots[FingerIndex].StartPos;
        float Sens = ScopeSensitivityScalers[(int)ActiveOptic] * HipfireSensitivity;
        CameraControlRotation.Yaw += Delta.X * 180.0f * Sens;
        CameraControlRotation.Pitch += (-Delta.Y) * 120.0f * Sens;
        CameraControlRotation.Pitch = FMath::Clamp(CameraControlRotation.Pitch, -85.0f, 85.0f);
        TouchSlots[FingerIndex].StartPos = ScreenNormalizedPos;
    }
}

void UIUMobileTouchInputSubsystem::OnTouchEndedOrCancelled(int32 FingerIndex)
{
    if (FingerIndex < 0 || FingerIndex >= 10) return;
    if (FingerIndex == 0)
    {
        if (!bSprintLocked)
        {
            AnalogMoveVector = FVector2D::ZeroVector;
        }
        else
        {
            AnalogMoveVector = FVector2D(0.0f, 1.0f);
        }
    }

    const FString Act = TouchSlots[FingerIndex].BoundAction;
    if (Act == "BTN_FIRE_LEFT") bLeftFirePressed = false;
    if (Act == "BTN_FIRE_RIGHT") bRightFirePressed = false;

    TouchSlots[FingerIndex].bActive = false;
    TouchSlots[FingerIndex].BoundAction = "";
}

int32 UIUMobileTouchInputSubsystem::GetActiveFingerCount() const
{
    int32 Count = 0;
    for (int i = 0; i < 10; ++i)
    {
        if (TouchSlots[i].bActive) ++Count;
    }
    return Count;
}

bool UIUMobileTouchInputSubsystem::RequestPostureChange(EIUMobilePosture NewPosture)
{
    CurrentPosture = NewPosture;
    if (CurrentPosture == EIUMobilePosture::Prone)
    {
        bSprintLocked = false;
    }
    return true;
}

float UIUMobileTouchInputSubsystem::GetCurrentPostureMaxSpeedCmPerSec() const
{
    switch (CurrentPosture)
    {
        case EIUMobilePosture::Prone:    return 120.0f;
        case EIUMobilePosture::Crouched: return 260.0f;
        case EIUMobilePosture::Standing: return 500.0f;
    }
    return 500.0f;
}

bool UIUMobileTouchInputSubsystem::RequestSprintState(bool bSprint)
{
    if (CurrentPosture == EIUMobilePosture::Prone)
    {
        bSprintLocked = false;
        return false;
    }
    bSprintLocked = bSprint;
    return true;
}

float UIUMobileTouchInputSubsystem::SolveWallSafePeekOffset(int32 DirectionSign, float WallClearanceCm, float DesiredPeekOffsetCm)
{
    const float SafetyBufferCm = 18.0f;
    float MaxAllowedOffset = FMath::Max(0.0f, WallClearanceCm - SafetyBufferCm);
    float TargetOffset = FMath::Min(DesiredPeekOffsetCm, MaxAllowedOffset);
    return TargetOffset * (DirectionSign >= 0 ? 1.0f : -1.0f);
}

void UIUMobileTouchInputSubsystem::ProcessGyroscopeSensorSample(const FVector& AngularVelocityDegPerSec, float DeltaTime)
{
    if (GyroMode == EIUGyroscopeMode::Off) return;
    if (GyroMode == EIUGyroscopeMode::WhileADS && !bADSPressed) return;

    CameraControlRotation.Yaw += AngularVelocityDegPerSec.Z * DeltaTime * GyroscopeSensitivity;
    CameraControlRotation.Pitch += AngularVelocityDegPerSec.Y * DeltaTime * GyroscopeSensitivity;
    CameraControlRotation.Pitch = FMath::Clamp(CameraControlRotation.Pitch, -85.0f, 85.0f);
}

bool UIUMobileTouchInputSubsystem::CustomizeHudWidgetSlot(const FString& ElementId, float NormalizedX, float NormalizedY, float Scale, float Opacity, bool bVisible)
{
    FIUHudWidgetElementSlot Slot;
    Slot.ElementId = ElementId;
    Slot.NormalizedX = FMath::Clamp(NormalizedX, 0.0f, 1.0f);
    Slot.NormalizedY = FMath::Clamp(NormalizedY, 0.0f, 1.0f);
    Slot.ScaleMultiplier = FMath::Clamp(Scale, 0.5f, 2.0f);
    Slot.Opacity = FMath::Clamp(Opacity, 0.1f, 1.0f);
    Slot.bIsVisible = bVisible;

    ActiveHudSlots[ElementId] = Slot;
    return true;
}

bool UIUMobileTouchInputSubsystem::SaveLayoutToProfileSlot(int32 ProfileSlotIndex)
{
    if (ProfileSlotIndex < 1 || ProfileSlotIndex > 5) return false;
    SavedProfiles[ProfileSlotIndex] = ActiveHudSlots;
    return true;
}

bool UIUMobileTouchInputSubsystem::LoadLayoutFromProfileSlot(int32 ProfileSlotIndex)
{
    auto It = SavedProfiles.find(ProfileSlotIndex);
    if (It == SavedProfiles.end()) return false;
    ActiveHudSlots = It->second;
    return true;
}

void UIUMobileTouchInputSubsystem::LoadLayoutPreset(EIUMobileLayoutPreset Preset)
{
    ActiveHudSlots.clear();
    // Default Two-Finger layout
    CustomizeHudWidgetSlot("BTN_JOYSTICK",   0.16f, 0.72f, 1.0f, 0.85f, true);
    CustomizeHudWidgetSlot("BTN_FIRE_RIGHT", 0.84f, 0.68f, 1.25f, 0.95f, true);
    CustomizeHudWidgetSlot("BTN_FIRE_LEFT",  0.15f, 0.25f, 1.0f, 0.75f, Preset != EIUMobileLayoutPreset::TwoFingerThumb);
    CustomizeHudWidgetSlot("BTN_ADS_SCOPE",  0.88f, 0.44f, 1.1f, 0.90f, true);
    CustomizeHudWidgetSlot("BTN_JUMP",       0.93f, 0.78f, 1.0f, 0.85f, true);
    CustomizeHudWidgetSlot("BTN_CROUCH",     0.83f, 0.86f, 1.0f, 0.85f, true);
    CustomizeHudWidgetSlot("BTN_PRONE",      0.74f, 0.90f, 0.95f, 0.85f, true);
    CustomizeHudWidgetSlot("BTN_RELOAD",     0.75f, 0.74f, 1.0f, 0.85f, true);
    CustomizeHudWidgetSlot("BTN_PEEK_LEFT",  0.22f, 0.46f, 0.95f, 0.80f, true);
    CustomizeHudWidgetSlot("BTN_PEEK_RIGHT", 0.29f, 0.46f, 0.95f, 0.80f, true);
    CustomizeHudWidgetSlot("PANEL_WEAPONS",  0.50f, 0.88f, 1.0f, 0.90f, true);
    CustomizeHudWidgetSlot("PANEL_SQUAD",    0.10f, 0.12f, 1.0f, 0.85f, true);
    CustomizeHudWidgetSlot("PANEL_MINIMAP",  0.88f, 0.14f, 1.0f, 0.90f, true);
    CustomizeHudWidgetSlot("BAR_COMPASS",    0.50f, 0.05f, 1.0f, 0.85f, true);

    if (Preset == EIUMobileLayoutPreset::FourFingerClaw)
    {
        // 4-finger claw: Left index fire at top-left, Right index ADS at top-right
        CustomizeHudWidgetSlot("BTN_FIRE_LEFT", 0.14f, 0.20f, 1.35f, 0.90f, true);
        CustomizeHudWidgetSlot("BTN_ADS_SCOPE", 0.86f, 0.22f, 1.35f, 0.90f, true);
    }
}

const FIUHudWidgetElementSlot* UIUMobileTouchInputSubsystem::GetHudWidgetSlot(const FString& ElementId) const
{
    auto It = ActiveHudSlots.find(ElementId);
    if (It != ActiveHudSlots.end())
    {
        return &It->second;
    }
    return nullptr;
}

void UIUMobileTouchInputSubsystem::NotifyVehiclePossessionChanged(bool bInVehicle)
{
    bVehicleHudActive = bInVehicle;
}

std::vector<EIUContextActionType> UIUMobileTouchInputSubsystem::EvaluateAvailableContextActions(
    bool bNearGroundLoot, bool bNearVehicle, bool bNearDoor, bool bNearKnockedAlly, bool bIsDriver, bool bIsPassenger, float DistanceCm)
{
    std::vector<EIUContextActionType> OutActions;
    if (bIsDriver || bIsPassenger)
    {
        OutActions.push_back(EIUContextActionType::ExitVehicle);
        return OutActions;
    }

    if (bNearVehicle && DistanceCm <= 250.0f)
    {
        OutActions.push_back(EIUContextActionType::EnterVehicle);
    }
    if (bNearGroundLoot && DistanceCm <= 180.0f)
    {
        OutActions.push_back(EIUContextActionType::PickupLoot);
    }
    if (bNearKnockedAlly && DistanceCm <= 160.0f)
    {
        OutActions.push_back(EIUContextActionType::ReviveTeammate);
    }
    if (bNearDoor && DistanceCm <= 180.0f)
    {
        OutActions.push_back(EIUContextActionType::OpenDoor);
    }

    return OutActions;
}

FIUQuantizedNetInputPacket UIUMobileTouchInputSubsystem::BuildQuantizedNetworkInputPacket() const
{
    FIUQuantizedNetInputPacket Pkt;
    Pkt.QuantizedMoveX = (int8)FMath::Clamp((int)(AnalogMoveVector.X * 100.0f), -100, 100);
    Pkt.QuantizedMoveY = (int8)FMath::Clamp((int)(AnalogMoveVector.Y * 100.0f), -100, 100);
    Pkt.QuantizedPitch = (int16)(CameraControlRotation.Pitch * 100.0f);
    Pkt.QuantizedYaw   = (int16)(CameraControlRotation.Yaw * 100.0f);

    uint8 Mask = 0;
    if (bLeftFirePressed || bRightFirePressed) Mask |= (1 << 0);
    if (bADSPressed) Mask |= (1 << 1);
    if (bSprintLocked) Mask |= (1 << 2);
    if (CurrentPosture == EIUMobilePosture::Crouched) Mask |= (1 << 3);
    if (CurrentPosture == EIUMobilePosture::Prone) Mask |= (1 << 4);
    Pkt.ButtonBitmask = Mask;

    return Pkt;
}

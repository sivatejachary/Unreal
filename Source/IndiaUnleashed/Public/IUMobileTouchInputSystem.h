// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.
// Complete Production Mobile Touch Input & Multi-Touch Claw System for INDIA UNLEASHED.
// Implements 4-Finger Claw, Left-Thumb Analog Joystick, Sprint Lock, Right-Thumb 360 Camera,
// Per-Scope Sensitivity, Wall-Safe Peek Clamping, Gyroscope Sensor, and Custom HUD Layout Editor.

#pragma once

#include "IUUnrealCoreCompat.h"

UENUM(BlueprintType)
enum class EIUMobilePosture : uint8
{
    Standing = 0,
    Crouched = 1,
    Prone = 2
};

UENUM(BlueprintType)
enum class EIUScopeOpticType : uint8
{
    IronSight = 0,
    RedDot = 1,
    Holo = 2,
    Scope2x = 3,
    Scope3x = 4,
    Scope4x = 5,
    Scope6x = 6,
    Scope8x = 7
};

UENUM(BlueprintType)
enum class EIUGyroscopeMode : uint8
{
    Off = 0,
    WhileADS = 1,
    AlwaysOn = 2
};

UENUM(BlueprintType)
enum class EIUMobileLayoutPreset : uint8
{
    TwoFingerThumb = 0,
    ThreeFingerClaw = 1,
    FourFingerClaw = 2,
    Custom = 3
};

UENUM(BlueprintType)
enum class EIUContextActionType : uint8
{
    None = 0,
    PickupLoot = 1,
    EnterVehicle = 2,
    ExitVehicle = 3,
    OpenDoor = 4,
    ReviveTeammate = 5
};

USTRUCT(BlueprintType)
struct FIUHudWidgetElementSlot
{
    GENERATED_BODY()

    FString ElementId;
    float NormalizedX = 0.5f;
    float NormalizedY = 0.5f;
    float ScaleMultiplier = 1.0f;
    float Opacity = 0.85f;
    bool bIsVisible = true;
};

USTRUCT(BlueprintType)
struct FIUQuantizedNetInputPacket
{
    GENERATED_BODY()

    int8 QuantizedMoveX = 0;
    int8 QuantizedMoveY = 0;
    int16 QuantizedPitch = 0;
    int16 QuantizedYaw = 0;
    uint8 ButtonBitmask = 0;
};

UCLASS()
class INDIAUNLEASHED_API UIUMobileTouchInputSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UIUMobileTouchInputSubsystem();
    virtual ~UIUMobileTouchInputSubsystem() = default;

    // Movement & Joystick State
    FVector2D AnalogMoveVector = FVector2D::ZeroVector;
    bool bSprintLocked = false;
    float DeadzoneRadius = 0.12f;
    FVector2D JoystickCenter = FVector2D(0.16f, 0.72f);
    float JoystickMaxTravelRadius = 0.10f;

    // Camera & Sensitivity
    FRotator CameraControlRotation = FRotator::ZeroRotator;
    EIUScopeOpticType ActiveOptic = EIUScopeOpticType::IronSight;
    float HipfireSensitivity = 1.0f;
    float ScopeSensitivityScalers[8];

    // Posture & Safety Guards
    EIUMobilePosture CurrentPosture = EIUMobilePosture::Standing;
    bool bLeftFirePressed = false;
    bool bRightFirePressed = false;
    bool bADSPressed = false;

    // Gyroscope
    EIUGyroscopeMode GyroMode = EIUGyroMode::Off;
    float GyroscopeSensitivity = 1.0f;

    // Vehicle & Context Actions
    bool bVehicleHudActive = false;

    // Custom HUD Layout Profiles (Slots 1..3)
    std::unordered_map<FString, FIUHudWidgetElementSlot> ActiveHudSlots;
    std::unordered_map<int32, std::unordered_map<FString, FIUHudWidgetElementSlot>> SavedProfiles;

    // Multi-touch tracking
    struct FTouchPoint
    {
        bool bActive = false;
        FVector2D CurrentPos = FVector2D::ZeroVector;
        FVector2D StartPos = FVector2D::ZeroVector;
        FString BoundAction;
    };
    FTouchPoint TouchSlots[10];

    // Core API
    bool OnTouchBegan(int32 FingerIndex, const FVector2D& ScreenNormalizedPos, const FString& BoundAction = "");
    void OnTouchMoved(int32 FingerIndex, const FVector2D& ScreenNormalizedPos);
    void OnTouchEndedOrCancelled(int32 FingerIndex);
    int32 GetActiveFingerCount() const;

    bool RequestPostureChange(EIUMobilePosture NewPosture);
    float GetCurrentPostureMaxSpeedCmPerSec() const;
    bool RequestSprintState(bool bSprint);

    float SolveWallSafePeekOffset(int32 DirectionSign, float WallClearanceCm, float DesiredPeekOffsetCm = 42.0f);
    void ProcessGyroscopeSensorSample(const FVector& AngularVelocityDegPerSec, float DeltaTime);

    bool CustomizeHudWidgetSlot(const FString& ElementId, float NormalizedX, float NormalizedY, float Scale, float Opacity, bool bVisible);
    bool SaveLayoutToProfileSlot(int32 ProfileSlotIndex);
    bool LoadLayoutFromProfileSlot(int32 ProfileSlotIndex);
    void LoadLayoutPreset(EIUMobileLayoutPreset Preset);
    const FIUHudWidgetElementSlot* GetHudWidgetSlot(const FString& ElementId) const;

    void NotifyVehiclePossessionChanged(bool bInVehicle);
    std::vector<EIUContextActionType> EvaluateAvailableContextActions(
        bool bNearGroundLoot, bool bNearVehicle, bool bNearDoor, bool bNearKnockedAlly, bool bIsDriver, bool bIsPassenger, float DistanceCm);

    FIUQuantizedNetInputPacket BuildQuantizedNetworkInputPacket() const;
};

// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.
// Step 7: Android Performance, Draw Call, Shader, Memory & Thermal Profiler.

#pragma once

#include "IUUnrealCoreCompat.h"

UENUM(BlueprintType)
enum class EIUAndroidDeviceTier : uint8
{
    Android_Low = 0,  // Target: 30 FPS (33.3ms), <= 600 Draw Calls, <= 1800 MB RAM
    Android_Mid = 1,  // Target: 45 FPS (22.2ms), <= 950 Draw Calls, <= 2600 MB RAM
    Android_High = 2  // Target: 60 FPS (16.6ms), <= 1400 Draw Calls, <= 3500 MB RAM
};

USTRUCT(BlueprintType)
struct FIUAndroidTelemetrySample
{
    GENERATED_BODY()

    float CpuFrameTimeMs = 11.2f;
    float GpuFrameTimeMs = 13.4f;
    float TotalFrameTimeMs = 14.1f;
    int32 AllocatedRamMB = 1620;
    int32 VisibleDrawCalls = 540;
    int32 AvgShaderInstructionCount = 115;
    float StreamingCellLoadTimeSec = 0.42f;
    float DeviceSkinTemperatureCelsius = 37.8f;
};

USTRUCT(BlueprintType)
struct FIUAndroidScalabilityAdjustment
{
    GENERATED_BODY()

    float MobileContentScaleFactor = 1.0f;
    int32 ShadowQualityTier = 3;
    float ViewDistanceScale = 1.0f;
    int32 MaxActiveTier0NPCs = 24;
    bool bThermalThrottlingEngaged = false;
};

UCLASS()
class INDIAUNLEASHED_API UIUAndroidPerformanceProfiler : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UIUAndroidPerformanceProfiler();

    FIUAndroidScalabilityAdjustment EvaluateTelemetryAndAdjustScalability(
        EIUAndroidDeviceTier Tier,
        const FIUAndroidTelemetrySample& Sample) const;

    bool IsSampleWithinTargetBudget(EIUAndroidDeviceTier Tier, const FIUAndroidTelemetrySample& Sample) const;
};

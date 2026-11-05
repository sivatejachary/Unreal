// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.

#include "../Public/IUAndroidProfiler.h"

UIUAndroidPerformanceProfiler::UIUAndroidPerformanceProfiler() = default;

FIUAndroidScalabilityAdjustment UIUAndroidPerformanceProfiler::EvaluateTelemetryAndAdjustScalability(
    EIUAndroidDeviceTier Tier,
    const FIUAndroidTelemetrySample& Sample) const
{
    FIUAndroidScalabilityAdjustment Out;

    switch (Tier)
    {
        case EIUAndroidDeviceTier::Android_Low:
            Out.MobileContentScaleFactor = 0.75f;
            Out.ShadowQualityTier = 1;
            Out.ViewDistanceScale = 0.60f;
            Out.MaxActiveTier0NPCs = 8;
            break;
        case EIUAndroidDeviceTier::Android_Mid:
            Out.MobileContentScaleFactor = 0.90f;
            Out.ShadowQualityTier = 2;
            Out.ViewDistanceScale = 0.80f;
            Out.MaxActiveTier0NPCs = 16;
            break;
        case EIUAndroidDeviceTier::Android_High:
            Out.MobileContentScaleFactor = 1.00f;
            Out.ShadowQualityTier = 3;
            Out.ViewDistanceScale = 1.00f;
            Out.MaxActiveTier0NPCs = 24;
            break;
    }

    // Engage automatic thermal mitigation if skin temperature >= 42.0 C
    if (Sample.DeviceSkinTemperatureCelsius >= 42.0f)
    {
        Out.bThermalThrottlingEngaged = true;
        Out.MobileContentScaleFactor = FMath::Max(0.65f, Out.MobileContentScaleFactor - 0.15f);
        Out.ShadowQualityTier = FMath::Max(1, Out.ShadowQualityTier - 1);
        Out.MaxActiveTier0NPCs = FMath::Max(6, Out.MaxActiveTier0NPCs / 2);
    }

    return Out;
}

bool UIUAndroidPerformanceProfiler::IsSampleWithinTargetBudget(EIUAndroidDeviceTier Tier, const FIUAndroidTelemetrySample& Sample) const
{
    if (Sample.DeviceSkinTemperatureCelsius >= 45.0f || Sample.StreamingCellLoadTimeSec > 1.5f)
    {
        return false;
    }

    switch (Tier)
    {
        case EIUAndroidDeviceTier::Android_Low:
            return (Sample.TotalFrameTimeMs <= 33.34f && Sample.VisibleDrawCalls <= 600 && Sample.AllocatedRamMB <= 1800);
        case EIUAndroidDeviceTier::Android_Mid:
            return (Sample.TotalFrameTimeMs <= 22.23f && Sample.VisibleDrawCalls <= 950 && Sample.AllocatedRamMB <= 2600);
        case EIUAndroidDeviceTier::Android_High:
            return (Sample.TotalFrameTimeMs <= 16.68f && Sample.VisibleDrawCalls <= 1400 && Sample.AllocatedRamMB <= 3500);
        default:
            return false;
    }
}

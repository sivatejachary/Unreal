// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.

using UnrealBuildTool;

public class IndiaUnleashed : ModuleRules
{
    public IndiaUnleashed(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        bEnableExceptions = true;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "NetCore",
            "PhysicsCore",
            "UMG",
            "CommonUI"
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "Slate",
            "SlateCore",
            "OnlineSubsystem",
            "OnlineSubsystemUtils"
        });
    }
}

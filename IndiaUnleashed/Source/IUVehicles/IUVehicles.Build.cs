// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.

using UnrealBuildTool;

public class IUVehicles : ModuleRules
{
    public IUVehicles(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "ChaosVehicles",
            "PhysicsCore",
            "IndiaUnleashed"
        });
    }
}

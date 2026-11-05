// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.

using UnrealBuildTool;

public class IUWorld : ModuleRules
{
    public IUWorld(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "Landscape",
            "NavigationSystem",
            "AIModule",
            "IndiaUnleashed"
        });
    }
}

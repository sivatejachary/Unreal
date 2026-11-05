// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.

using UnrealBuildTool;

public class IUCombat : ModuleRules
{
    public IUCombat(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "PhysicsCore",
            "IndiaUnleashed"
        });
    }
}

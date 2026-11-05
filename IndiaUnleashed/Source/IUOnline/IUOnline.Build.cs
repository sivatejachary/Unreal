// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.

using UnrealBuildTool;

public class IUOnline : ModuleRules
{
    public IUOnline(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "OnlineSubsystem",
            "OnlineSubsystemUtils",
            "HTTP",
            "Json",
            "IndiaUnleashed"
        });
    }
}

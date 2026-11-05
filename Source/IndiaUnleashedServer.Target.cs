// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.
// Dedicated Server build target for Multiplayer City & 100-Player Battle Royale

using UnrealBuildTool;
using System.Collections.Generic;

[SupportedPlatforms(UnrealPlatformClass.Server)]
public class IndiaUnleashedServerTarget : TargetRules
{
    public IndiaUnleashedServerTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Server;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
        CppStandard = CppStandardVersion.Cpp20;
        bUseChecksInShipping = true;
        bUseLoggingInShipping = true;

        ExtraModuleNames.AddRange(new string[] {
            "IndiaUnleashed",
            "IUOnline",
            "IUWorld",
            "IUVehicles",
            "IUCombat",
            "IUMissions",
            "IUBattleRoyale"
        });
    }
}

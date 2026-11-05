// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class IndiaUnleashedClientTarget : TargetRules
{
    public IndiaUnleashedClientTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Client;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
        CppStandard = CppStandardVersion.Cpp20;

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

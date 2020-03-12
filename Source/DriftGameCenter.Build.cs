// Copyright 2015-2016 Directive Games Limited - All Rights Reserved

using UnrealBuildTool;
using System.IO;

public class DriftGameCenter : ModuleRules
{
	public DriftGameCenter(ReadOnlyTargetRules TargetRules) : base(TargetRules)
    {
#if UE_4_24_OR_LATER
		ShadowVariableWarningLevel = WarningLevel.Off;
#else
		bEnableShadowVariableWarnings = false;
#endif

		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
			new string[] {
			}
			);
				

		PrivateIncludePaths.AddRange(
			new string[] {
			}
			);

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "Projects",
			}
			);

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "HTTP",
                "Json",
                "JsonUtilities",
                "OnlineSubsystem",
                "OnlineSubsystemUtils",
                "Drift",
            }
            );

        if (TargetRules.Platform == UnrealTargetPlatform.Mac) {
            PublicFrameworks.AddRange(
                new string[]
                {
                    "Gamekit",
                }
            );
        }
    }
}

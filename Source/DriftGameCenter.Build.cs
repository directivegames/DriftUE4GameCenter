// Copyright 2015-2016 Directive Games Limited - All Rights Reserved

using UnrealBuildTool;
using System.IO;

public class DriftGameCenter : ModuleRules
{
	public DriftGameCenter(ReadOnlyTargetRules TargetRules) : base(TargetRules)
    {
		bEnableShadowVariableWarnings = false;
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
			new string[] {
				"DriftGameCenter/Public",		
			}
			);
				

		PrivateIncludePaths.AddRange(
			new string[] {
				"DriftGameCenter/Private",
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
    }
}

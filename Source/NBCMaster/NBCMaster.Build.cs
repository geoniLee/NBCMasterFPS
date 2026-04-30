// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class NBCMaster : ModuleRules
{
	public NBCMaster(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}

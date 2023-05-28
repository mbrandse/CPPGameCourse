// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CPPCourse : ModuleRules
{
	public CPPCourse(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"HeadMountedDisplay", 
			"AIModule",            
			"UMG",
            "Slate",
            "SlateCore",
			"Niagara"});
	}
}

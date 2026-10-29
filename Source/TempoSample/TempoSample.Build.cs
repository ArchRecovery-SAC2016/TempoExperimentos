// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TempoSample : TempoModuleRules
{
	public TempoSample(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		bEnableExceptions = true;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			// Unreal
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			// Tempo
			"TempoCore",
			"TempoSensors",
			"TempoSensorsShaders",
			"TempoROS",
			"rclcpp",
		});

		PrivateDependencyModuleNames.AddRange(new string[] {  });
	}
}

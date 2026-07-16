// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DashAction : ModuleRules
{
	public DashAction(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"DashAction",
			"DashAction/Variant_Platforming",
			"DashAction/Variant_Platforming/Animation",
			"DashAction/Variant_Combat",
			"DashAction/Variant_Combat/AI",
			"DashAction/Variant_Combat/Animation",
			"DashAction/Variant_Combat/Gameplay",
			"DashAction/Variant_Combat/Interfaces",
			"DashAction/Variant_Combat/UI",
			"DashAction/Variant_SideScrolling",
			"DashAction/Variant_SideScrolling/AI",
			"DashAction/Variant_SideScrolling/Gameplay",
			"DashAction/Variant_SideScrolling/Interfaces",
			"DashAction/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}

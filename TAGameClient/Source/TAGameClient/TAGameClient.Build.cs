// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class TAGameClient : ModuleRules
{
	public TAGameClient(ReadOnlyTargetRules Target) : base(Target)
	{
		//PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG", "SlateCore", "NavigationSystem", "Navmesh", "UnrealEd" });

		//"LevelEditor", "EditorStyle", "EditorWidgets",

		PCHUsage = PCHUsageMode.NoSharedPCHs;
		PrivatePCHHeaderFile = "PCH.h";
		CppStandard = CppStandardVersion.Cpp17;

		PrivateDependencyModuleNames.AddRange(new string[] { });

		// ../ => TAGameClient
		PrivateIncludePaths.Add("../Source/TAGameClient/Public/Common/NavigationSystem");

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true

		// intellisense 너무 느릴때
		/*
			Configuration Properties -> NMake -> Additional Options
			Enter: /Yu and press OK or Apply.
			Wait until the 'working' symbol goes away, the intellisense should be much faster now.
		*/
	}
}

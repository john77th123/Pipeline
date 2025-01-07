// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AnimExporter : ModuleRules
{
	public AnimExporter(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
            System.Array.Empty<string>()
            );
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "SequencerScripting",
                "SequencerScriptingEditor",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "CoreUObject",
                "ControlRig",
                "DesktopPlatform",
                "Engine",
                "LevelSequence",
                "LevelSequenceEditor",
                "MovieScene",
                "Sequencer",
                "SequencerCore",
				"Slate",
				"UnrealEd"
				// ... add private dependencies that you statically link with here ...	
			}
            );
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}

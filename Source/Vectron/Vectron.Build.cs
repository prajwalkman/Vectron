// Some copyright should be here...

using UnrealBuildTool;

public class Vectron : ModuleRules
{
	public Vectron(TargetInfo Target)
	{

		PublicIncludePaths.AddRange(
			new string[] {
				"Vectron/Public"

				// ... add public include paths required here ...
			}
			);


		PrivateIncludePaths.AddRange(
			new string[] {
				"Vectron/Private",

				// ... add other private include paths required here ...
			}
			);

        PrivateIncludePathModuleNames.AddRange(
            new string[] {
                "AssetRegistry",
                "AssetTools",
                "CollectionManager",
                "PropertyEditor"
            }
        );

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"InputCore",
				"Engine",
				"CoreUObject",
                "ContentBrowser",


				// ... add other public dependencies that you statically link with here ...
			}
			);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"UnrealEd",
				"LevelEditor",
				"Slate", "SlateCore",
                "ContentBrowser",
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

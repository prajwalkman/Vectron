// Some copyright should be here...

#pragma once

#include "ModuleManager.h"
#include "VectorField/VectorFieldStatic.h"

struct FFGAContents
{
	int32 GridX;
	int32 GridY;
	int32 GridZ;
	FBox Bounds;
	TArray<FVector> Vectors;
};

class FVectronModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command.*/
	void PluginButtonClicked();
	void OtherPluginButtonClicked();
	
private:

	void AddToolbarExtension(class FToolBarBuilder &);
	void AddMenuExtension(class FMenuBuilder &);

	void InjectVolumeIntoScene();
	FFGAContents* m_escrowFga;
	UVectorFieldStatic* m_vectorField;

	void ExportEscorwToFile(FString);

	TSharedPtr<class FUICommandList> PluginCommands;
};
// Some copyright should be here...
#pragma once

#include "SlateBasics.h"
#include "VectronStyle.h"

class FVectronCommands : public TCommands<FVectronCommands>
{
public:

	FVectronCommands()
		: TCommands<FVectronCommands>(TEXT("Vectron"), NSLOCTEXT("Contexts", "Vectron", "Vectron Plugin"), NAME_None, FVectronStyle::GetStyleSetName())
	{
	}

	virtual void RegisterCommands() override
	{
		UI_COMMAND(PluginAction, "Vectron", "Execute Vectron action", EUserInterfaceActionType::Button, FInputGesture());
	}

	TSharedPtr< FUICommandInfo > PluginAction;
};
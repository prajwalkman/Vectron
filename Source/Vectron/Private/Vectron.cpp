#include "VectronPrivatePCH.h"

#include "SlateBasics.h"
#include "SlateExtras.h"

#include "VectronStyle.h"
#include "VectronCommands.h"

#include "LevelEditor.h"


static const FName VectronTabName("Vectron");

#define LOCTEXT_NAMESPACE "FVectronModule"

#define DLOG(text) if (GLog) GLog->Log(text)
#define DLOGN(num) if (GLog) GLog->Log(FString::FromInt(num))
#define DLOGF(flt) if (GLog) GLog->Log(FString::SanitizeFloat(flt))

/**
* Walks the stream looking for the specified character. Replaces the character
* with NULL and returns the next character in the stream.
*/
static TCHAR* ParseUntil(TCHAR* Stream, TCHAR Separator)
{
	check(Stream != NULL);

	while (*Stream != 0 && *Stream != Separator)
	{
		Stream++;
	}
	if (*Stream == Separator)
	{
		*Stream++ = 0;
	}
	return Stream;
}

/**
* Returns the next value in a CSV as an integer.
*/
static TCHAR* ParseIntCsv(TCHAR* Stream, int32* OutValue)
{
	check(Stream != NULL);
	check(OutValue != NULL);

	const TCHAR* Token = Stream;
	Stream = ParseUntil(Stream, TEXT(','));
	*OutValue = FCString::Atoi(Token);

	return Stream;
}

/**
* Returns the next value in a CSV as a float.
*/
static TCHAR* ParseFloatCsv(TCHAR* Stream, float* OutValue)
{
	check(Stream != NULL);
	check(OutValue != NULL);

	const TCHAR* Token = Stream;
	Stream = ParseUntil(Stream, TEXT(','));
	*OutValue = FCString::Atof(Token);

	return Stream;
}

/**
* Parse an FGA file.
*/
static bool ParseFGA(
	FFGAContents* OutContents,
	TCHAR* Stream,
	FFeedbackContext* Warn)
{
	int32 GridX, GridY, GridZ;
	FBox Bounds;

	check(Stream != NULL);
	check(OutContents != NULL);

	// Parse the grid size.
	Stream = ParseIntCsv(Stream, &GridX);
	Stream = ParseIntCsv(Stream, &GridY);
	Stream = ParseIntCsv(Stream, &GridZ);

	// Parse the bounding box.
	Stream = ParseFloatCsv(Stream, &Bounds.Min.X);
	Stream = ParseFloatCsv(Stream, &Bounds.Min.Y);
	Stream = ParseFloatCsv(Stream, &Bounds.Min.Z);
	Stream = ParseFloatCsv(Stream, &Bounds.Max.X);
	Stream = ParseFloatCsv(Stream, &Bounds.Max.Y);
	Stream = ParseFloatCsv(Stream, &Bounds.Max.Z);

	// Make sure there is more to read.
	if (*Stream == 0)
	{
		Warn->Logf(ELogVerbosity::Warning, TEXT("Unexpected end of file."));
		return false;
	}

	// Make sure the grid size is acceptable.
	const int32 MaxGridSize = 128;
	if (GridX < 0 || GridX > MaxGridSize
		|| GridY < 0 || GridY > MaxGridSize
		|| GridZ < 0 || GridZ > MaxGridSize)
	{
		Warn->Logf(ELogVerbosity::Warning, TEXT("Invalid grid size: %dx%dx%d"), GridX, GridY, GridZ);
		return false;
	}

	// Propagate grid size and bounds.
	OutContents->GridX = GridX;
	OutContents->GridY = GridY;
	OutContents->GridZ = GridZ;
	OutContents->Bounds = Bounds;
	OutContents->Bounds.IsValid = true;

	// Allocate storage for grid values.
	const int32 VectorCount = GridX * GridY * GridZ;
	const int32 ValueCount = VectorCount * 3;
	TArray<float> OValues;
	OValues.Empty(ValueCount);
	OValues.AddZeroed(ValueCount);
	OutContents->Vectors.Empty(VectorCount);
	OutContents->Vectors.AddZeroed(VectorCount);
	float* Values = OValues.GetData();
	float* ValuesEnd = Values + OValues.Num();

	// Parse each value.
	do
	{
		Stream = ParseFloatCsv(Stream, Values++);
	} while (*Stream && Values < ValuesEnd);

	// Check that the correct number of values have been read in.
	if (Values != ValuesEnd)
	{
		Warn->Logf(ELogVerbosity::Warning, TEXT("Expected %d values but only found %d in the file."),
			ValueCount, ValuesEnd - Values);
		return false;
	}

	// Check to see that the entire file has been parsed.
	if (*Stream != 0)
	{
		Warn->Logf(ELogVerbosity::Warning, TEXT("File contains additional information. This is not fatal but may mean the data has been truncated."));
	}

	for (uint16 i = 0; i < OValues.Num(); i += 3) {
		OutContents->Vectors[i / 3] = FVector(Values[i], Values[i + 1], Values[i + 2]);
	}

	return true;
}

void FVectronModule::StartupModule()
{
	// This code will execute after your module is loaded into memory (but after global variables are initialized, of course.)

	FVectronStyle::Initialize();
	FVectronStyle::ReloadTextures();

	FVectronCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FVectronCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FVectronModule::PluginButtonClicked),
		FCanExecuteAction());

	PluginCommands->MapAction(
		FVectronCommands::Get().OtherPluginAction,
		FExecuteAction::CreateRaw(this, &FVectronModule::OtherPluginButtonClicked),
		FCanExecuteAction());

	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	{
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
		MenuExtender->AddMenuExtension("WindowLayout", EExtensionHook::After, PluginCommands, FMenuExtensionDelegate::CreateRaw(this, &FVectronModule::AddMenuExtension));

		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	}

	{
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FVectronModule::AddToolbarExtension));

		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}
}

void FVectronModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	//if (m_escrowFga != nullptr) delete m_escrowFga;

	FVectronStyle::Shutdown();

	FVectronCommands::Unregister();
}

void FVectronModule::PluginButtonClicked()
{
	// Put your "OnButtonClicked" stuff here

	//if (m_escrowFga == nullptr)
	//{
	//	m_escrowFga = new FFGAContents();
	//}
	
	auto app = FPaths::GameContentDir() + "vf.fga";
	FString res;
	FFileHelper::LoadFileToString(res, *app);

	m_escrowFga = new FFGAContents();
	if (ParseFGA(m_escrowFga, (TCHAR*)*res, nullptr))
	{
		InjectVolumeIntoScene();
	}
	else
	{
		DLOG("Could not parse FGA file.");
	}
}

void FVectronModule::OtherPluginButtonClicked()
{
	DLOG("AS TEDDY COMMANDS!");
}

void FVectronModule::InjectVolumeIntoScene() {
	// This is where we show a visualization of our field with a custom box and custom billboards for vectors
	auto World = GEditor->GetEditorWorldContext().World();
	auto Level = World->GetCurrentLevel();
	int32 x = 0;
	int32 y = 0;
	int32 z = 0;
	float mult = 15.0;

	for (auto vec : m_escrowFga->Vectors)
	{
		FTransform ft = FTransform::Identity;
		FQuat nr = FQuat::MakeFromEuler(vec);
		ft.SetRotation(nr);
		ft.SetTranslation(FVector(x, y, z) * mult);
		ft.SetScale3D(FVector(0.1, 0.1, 0.5));

		// ft is the transform for our voxel

		x += 1;
		if (x > m_escrowFga->GridX)
		{
			x = 0;
			y += 1;
			if (y > m_escrowFga->GridY)
			{
				y = 0;
				z += 1;
			}
		}

	}
}

void FVectronModule::AddMenuExtension(FMenuBuilder& builder)
{
	{
		builder.AddMenuEntry(
			FVectronCommands::Get().PluginAction,
			NAME_None,
			FVectronCommands::Get().PluginAction->GetLabel(),
			FVectronCommands::Get().PluginAction->GetDescription(),
			FVectronCommands::Get().PluginAction->GetIcon(),
			NAME_None);
	}
}

void FVectronModule::AddToolbarExtension(FToolBarBuilder &builder)
{
	builder.AddToolBarButton(
		FVectronCommands::Get().PluginAction,
		NAME_None,
		FVectronCommands::Get().PluginAction->GetLabel(),
		FVectronCommands::Get().PluginAction->GetDescription(),
		FVectronCommands::Get().PluginAction->GetIcon(),
		NAME_None);

	builder.AddToolBarButton(
		FVectronCommands::Get().OtherPluginAction,
		NAME_None,
		FVectronCommands::Get().OtherPluginAction->GetLabel(),
		FVectronCommands::Get().OtherPluginAction->GetDescription(),
		FVectronCommands::Get().OtherPluginAction->GetIcon(),
		NAME_None);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FVectronModule, Vectron)
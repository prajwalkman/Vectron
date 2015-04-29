#include "VectronPrivatePCH.h"
#include "VectronBoundingBox.h"
#include "SlateBasics.h"
#include "SlateExtras.h"

#include "VectronStyle.h"
#include "VectronCommands.h"
#include "LevelEditor.h"
#include "Classes/VectronBoundingBox.h"
#include "Classes/VectronPrimitive.h"
#include "Internationalization/Text.h"

static const FName VectronTabName("Vectron");

#define LOCTEXT_NAMESPACE "FVectronModule"


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
		OutContents->Vectors[i / 3] = FVector(OValues[i], OValues[i + 1], OValues[i + 2]);
	}

	return true;
}

void FVectronModule::StartupModule()
{
	// This code will execute after your module is loaded into memory (but after global variables are initialized, of course.)

	FVectronStyle::Initialize();
	FVectronStyle::ReloadTextures();

	FVectronCommands::Register();

	//PluginCommands = MakeShareable(new FUICommandList);

	//PluginCommands->MapAction(
	//	FVectronCommands::Get().PluginAction,
	//	FExecuteAction::CreateRaw(this, &FVectronModule::PluginButtonClicked),
	//	FCanExecuteAction());

	//PluginCommands->MapAction(
	//	FVectronCommands::Get().OtherPluginAction,
	//	FExecuteAction::CreateRaw(this, &FVectronModule::OtherPluginButtonClicked),
	//	FCanExecuteAction());

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

	FVectronStyle::Shutdown();

	FVectronCommands::Unregister();
}

void FVectronModule::InjectVolumeIntoScene() {
	// This is where we show a visualization of our field with a custom box and custom billboards for vectors
	auto World = GEditor->GetEditorWorldContext().World();
	auto Level = World->GetCurrentLevel();
	m_activeActor = GEditor->AddActor(Level, AVectronBoundingBox::StaticClass(), FTransform::Identity);
	auto avbb = Cast<AVectronBoundingBox>(m_activeActor);
	ManualUpdateDelegate.BindUObject(avbb, &AVectronBoundingBox::ManualUpdate);
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

void FVectronModule::Import()
{
	FEditorDelegates::LoadSelectedAssetsIfNeeded.Broadcast();

	USelection *Selection = GEditor->GetSelectedObjects();
	if (Selection == nullptr) return;
	UObject *SelectedActor = Selection->GetTop<UObject>();
	if (SelectedActor == nullptr) return;
	if (SelectedActor->GetClass() != UVectorFieldStatic::StaticClass())
	{
		return;
	}
	auto pathName = SelectedActor->GetPathName();
	pathName = pathName.Replace(TEXT("/Game/"), TEXT(""));
	FString a, b;
	pathName.Split(TEXT("."), &a, &b);

	auto app = FPaths::GameContentDir() + a + ".fga";
	FString res;
	if (!FFileHelper::LoadFileToString(res, *app))
	{
		DLOG("Could not find file: " + app);
		return;
	}

	m_escrowFga = new FFGAContents();
	if (ParseFGA(m_escrowFga, (TCHAR*)*res, nullptr))
	{
		InjectVolumeIntoScene();
		holding = true;
	}
	else
	{
		DLOG("Could not parse FGA file.");
	}
}

void FVectronModule::Update()
{
	ManualUpdateDelegate.Execute();
	GEditor->RedrawAllViewports();
}

void FVectronModule::AddPrimitive()
{
	auto World = GEditor->GetEditorWorldContext().World();
	auto Level = World->GetCurrentLevel();
	GEditor->AddActor(Level, AVectronPrimitive::StaticClass(), FTransform::Identity);
}

void FVectronModule::Export()
{
	FEditorDelegates::LoadSelectedAssetsIfNeeded.Broadcast();

	USelection *Selection = GEditor->GetSelectedActors();
	AVectronBoundingBox *SelectedActor = Cast<AVectronBoundingBox>(Selection->GetTop<AVectronBoundingBox>());
	if (SelectedActor == NULL){
		DLOG("wrong class");
		return;
	}
	DLOG(SelectedActor->GetClass()->GetDesc());
	FFGAContents* fga = SelectedActor->getFFGAContents();
	FString file = FString::Printf(TEXT("%i, %i, %i,\n"), fga->GridX, fga->GridY, fga->GridZ);
	file += FString::Printf(TEXT("%f, %f, %f,\n"), fga->Bounds.Min.X, fga->Bounds.Min.Y, fga->Bounds.Min.Z);
	file += FString::Printf(TEXT("%f, %f, %f,\n"), fga->Bounds.Max.X, fga->Bounds.Max.Y, fga->Bounds.Max.Z);
	for (int32 i = 0; i < fga->Vectors.Num(); i++)
	{
		file += FString::Printf(TEXT("%f, %f, %f,\n"), fga->Vectors[i].X * 100, fga->Vectors[i].Y * 100, fga->Vectors[i].Z * 100);
	}
	//DLOG(file);
	auto appOut = FPaths::GameContentDir() + "vfnew.fga";
	FFileHelper::SaveStringToFile(file, *appOut);
}

void FVectronModule::AddToolbarExtension(FToolBarBuilder &builder)
{
	builder.AddSeparator();

	//builder.AddToolBarButton(
	//	FVectronCommands::Get().PluginAction,
	//	NAME_None,
	//	FText::FromString(FString("Load/Update")),
	//	FVectronCommands::Get().PluginAction->GetDescription(),
	//	FVectronCommands::Get().PluginAction->GetIcon(),
	//	NAME_None);

	//builder.AddToolBarButton(
	//	FVectronCommands::Get().OtherPluginAction,
	//	NAME_None,
	//	FText::FromString(FString("Save")),
	//	FVectronCommands::Get().OtherPluginAction->GetDescription(),
	//	FVectronCommands::Get().OtherPluginAction->GetIcon(),
	//	NAME_None);

	TSharedRef<FUICommandList> nothing(new FUICommandList());
	builder.AddComboButton(
		FUIAction(),
		FOnGetContent::CreateStatic(&FVectronModule::MenuCombo, nothing),
		FText::FromString("Vectron"),
		FText::FromString("Vectron Actions"),
		FVectronCommands::Get().PluginAction->GetIcon());
}

TSharedRef<SWidget> FVectronModule::MenuCombo(TSharedRef<FUICommandList> InCmdList)
{
	FMenuBuilder MenuBuilder(true, NULL);

	FUIAction LoadAction(FExecuteAction::CreateRaw(&FVectronModule::Get(), &FVectronModule::Import));
	MenuBuilder.AddMenuEntry(FText::FromString("Import"), FText::FromString("Load Vector Field selected in ContentBrowser"), FSlateIcon(), LoadAction);

	FUIAction AddPrimitive(FExecuteAction::CreateRaw(&FVectronModule::Get(), &FVectronModule::AddPrimitive));
	MenuBuilder.AddMenuEntry(FText::FromString("Add Effector"), FText::FromString("Adds a primitive that can affect the field"), FSlateIcon(), AddPrimitive);

	FUIAction UpdateAction(FExecuteAction::CreateRaw(&FVectronModule::Get(), &FVectronModule::Update));
	MenuBuilder.AddMenuEntry(FText::FromString("Update"), FText::FromString("Update loaded field with primitive influences"), FSlateIcon(), UpdateAction);

	FUIAction SaveAction(FExecuteAction::CreateRaw(&FVectronModule::Get(), &FVectronModule::Export));
	MenuBuilder.AddMenuEntry(FText::FromString("Export"), FText::FromString("Save FBoundingBox selected in scene"), FSlateIcon(), SaveAction);

	return MenuBuilder.MakeWidget();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FVectronModule, Vectron)
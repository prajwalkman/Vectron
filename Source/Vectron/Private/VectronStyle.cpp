// Some copyright should be here...
#include "VectronPrivatePCH.h"

#include "VectronStyle.h"
#include "SlateGameResources.h"

TSharedPtr< FSlateStyleSet > FVectronStyle::StyleInstance = NULL;

void FVectronStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FVectronStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FVectronStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("VectronStyle"));
	return StyleSetName;
}

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define TTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".ttf") ), __VA_ARGS__ )
#define OTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".otf") ), __VA_ARGS__ )

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);
const FVector2D Icon40x40(40.0f, 40.0f);

TSharedRef< FSlateStyleSet > FVectronStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("VectronStyle"));
	Style->SetContentRoot(FPaths::EnginePluginsDir() / TEXT("Vectron/Resources"));

	//Style->Set("ButtonIcon", new IMAGE_BRUSH(TEXT("ButtonIcon"), Icon40x40));
	Style->Set("Vectron.PluginAction", new IMAGE_BRUSH(TEXT("ButtonIcon"), Icon40x40));
	Style->Set("Vectron.OtherPluginAction", new IMAGE_BRUSH(TEXT("ButtonIcon"), Icon40x40));

	return Style;
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT

void FVectronStyle::ReloadTextures()
{
	FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
}

const ISlateStyle& FVectronStyle::Get()
{
	return *StyleInstance;
}

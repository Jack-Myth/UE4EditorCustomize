// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "UThemeInfoWindowFactory.h"
#include "Widgets/Images/SImage.h"
#include "Interfaces/IPluginManager.h"
#include "Brushes/SlateImageBrush.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Styling/SlateBrush.h"
#include "Framework/Application/SlateApplication.h"
#include "IImageWrapperModule.h"
#include "IImageWrapper.h"
#include "EditorCustomizeSetting.h"
#include "DesktopPlatformModule.h"
#include "Widgets/Layout/SConstraintCanvas.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "FUE4EditorCustomizeModule"

#define ADD_INFO_ITEM(INFO_TEXTBOX,INFO_NAME,EDITABLE,INFO_CHANGED_DELEGATE) \
+ SVerticalBox::Slot().AutoHeight()\
[\
SNew(SHorizontalBox)\
+ SHorizontalBox::Slot().FillWidth(1.f).MaxWidth(100.f)\
[\
SNew(STextBlock)\
.Text(INFO_NAME)\
]\
+ SHorizontalBox::Slot().FillWidth(1.f)\
[\
	SAssignNew(INFO_TEXTBOX,SEditableTextBox)\
	.OnTextChanged(INFO_CHANGED_DELEGATE)\
	.IsReadOnly(EDITABLE)\
]]

void UThemeInfoWindowFactory::BuildSlateBrush()
{
	FString NoImagePath = GetPluginResourcePath(IsEditable() ? "NoImageAddable.jpg" : "NoImage.jpg");
	FString RemoveImagePath = GetPluginResourcePath("RemoveImage.png");
	IconImageBrush = FSlateImageBrush(GetLocalTexture(NoImagePath), FVector2D(256.f, 256.f));
	IconImageBrush.Tiling = ESlateBrushTileType::NoTile;
	RemoveImageBrush = FSlateImageBrush(GetLocalTexture(RemoveImagePath), FVector2D(50.f, 50.f));
	RemoveImageBrush.Tiling = ESlateBrushTileType::NoTile;
	RemoveImageButton.SetPressed(RemoveImageBrush);
	RemoveImageButton.SetHovered(RemoveImageBrush);
	RemoveImageButton.SetNormal(RemoveImageBrush);
}

UThemeInfoWindowFactory::UThemeInfoWindowFactory(bool Editable)
{
	Name = Author = Intro = FText::FromString("None");
	InfoEditable = Editable;
	BuildSlateBrush();
	SAssignNew(UThemeInfoWindow, SWindow).ClientSize(FVector2D(500.f, 800.f)).SizingRule(ESizingRule::FixedSize).Title(LOCTEXT("UThemeInfo", "UTheme Infomation"))
		[
			SNew(SBorder).BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
			[
				SAssignNew(MainVerticlePanel, SVerticalBox)
				+ SVerticalBox::Slot().AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().AutoWidth().HAlign(HAlign_Fill).VAlign(VAlign_Fill)
		[
			SNew(SImage)
			.OnMouseButtonDown(FPointerEventHandler::CreateRaw(this, &UThemeInfoWindowFactory::OnThemeIconMouseDown))
		.Image(&IconImageBrush)
		]
	+ SHorizontalBox::Slot().FillWidth(1.f)
		[
			SAssignNew(MainInfoPanel, SVerticalBox)
			ADD_INFO_ITEM(NameBox, LOCTEXT("Name:", "Name:"), !IsEditable(), FOnTextChanged::CreateLambda([this](const FText& NewName) {Name = NewName; }))
		ADD_INFO_ITEM(SizeBox, LOCTEXT("Size:", "Size:"), true, FOnTextChanged())
		ADD_INFO_ITEM(EngineVersionBox, LOCTEXT("RequestEngine:", "Request Engine:"), true, FOnTextChanged())
		ADD_INFO_ITEM(VersionBox, LOCTEXT("UThemeVersion:", "UThemeVerison:"), true, FOnTextChanged())
		+ SVerticalBox::Slot().FillHeight(1.f).VAlign(VAlign_Bottom)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().FillWidth(1.f).MaxWidth(100.f)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("Author:", "Author:"))
		]
	+ SHorizontalBox::Slot().FillWidth(1.f)
		[
			SAssignNew(AuthorBox, SEditableTextBox)
			.OnTextChanged(FOnTextChanged::CreateLambda([this](const FText& NewAuthor) {Author = NewAuthor; }))
		.IsReadOnly(!IsEditable())
		]
		]
		]
		]
	+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Fill).VAlign(VAlign_Fill)
		[
			SNew(SScrollBox).Orientation(Orient_Horizontal)
			+ SScrollBox::Slot().VAlign(VAlign_Fill).HAlign(HAlign_Fill)
		[
			SAssignNew(PreviewPicBox, SHorizontalBox)
		]
		]
	+ SVerticalBox::Slot().AutoHeight()
		[
			SNew(STextBlock)
			.Text(LOCTEXT("Intro:", "Intro:"))
		]
	+ SVerticalBox::Slot().FillHeight(1.f)
		[
			SNew(SScrollBox).Orientation(EOrientation::Orient_Vertical)
			+ SScrollBox::Slot()
		[
			SAssignNew(IntroBox, SMultiLineEditableTextBox)
			.IsReadOnly(!IsEditable())
		.OnTextChanged(FOnTextChanged::CreateLambda([this](const FText& NewText) { Intro = NewText; }))
		]
		]
	+ SVerticalBox::Slot().FillHeight(1.f).VAlign(VAlign_Bottom)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().FillWidth(1.f).HAlign(HAlign_Right)
		[
			SNew(SButton)
			.Text(LOCTEXT("Comfirm", "Confirm"))
		.OnClicked(FOnClicked::CreateLambda([this]() {IsConfirmed = true; UThemeInfoWindow->RequestDestroyWindow(); return FReply::Handled(); }))
		]
	+ SHorizontalBox::Slot().AutoWidth()
		[
			SNew(SButton)
			.Text(LOCTEXT("Cancel", "Cancel"))
		.OnClicked(FOnClicked::CreateLambda([this]() {IsConfirmed = false; UThemeInfoWindow->RequestDestroyWindow(); return FReply::Handled(); }))
		]
		]
			]
		];
	if (IsEditable())
	{
		PreviewPicBox->AddSlot().VAlign(VAlign_Fill).HAlign(HAlign_Left).AutoWidth()
			[
				SAssignNew(AddPreviewImageButton, SButton).Text(LOCTEXT("AddPreview", "Add Preview"))
				.OnClicked(FOnClicked::CreateLambda([this]()
					{
						TArray<FString> PictureArray;
						if (FDesktopPlatformModule::Get()->OpenFileDialog(nullptr, LOCTEXT("SelectPicture", "Select Picture").ToString(), "", "",
							"All(*.JPG,*.JPEG,*.PNG,*.BMP)|*.jpg;*.jpeg;*.png;*.bmp|JPEG(*.JPG,*.JPEG)|*.jpg;*.jpeg|PNG(*.png)|*.png|BMP(*.bmp)|*.bmp", EFileDialogFlags::Multiple, PictureArray))
						{
							for (int i = 0; i < PictureArray.Num(); i++)
							{
								if (tmpImagePath.Contains(PictureArray[i]))
									continue;
								AddPreviewPicture(GetLocalTexture(PictureArray[i]), true, PictureArray[i]);
								tmpImagePath.Add(PictureArray[i]);
							}
						}
						return FReply::Handled();
					}))
			];
	}
}

UThemeInfoWindowFactory::~UThemeInfoWindowFactory()
{}

FReply UThemeInfoWindowFactory::OnThemeIconMouseDown(const FGeometry& Geometry, const FPointerEvent& PointerEvent)
{
	TArray<FString> PictureArray;
	if (!IsEditable())
		return FReply::Handled();
	if (FDesktopPlatformModule::Get()->OpenFileDialog(nullptr, LOCTEXT("SelectPicture", "Select Picture").ToString(), "", "",
		"All(*.JPG,*.JPEG,*.PNG,*.BMP)|*.jpg;*.jpeg;*.png;*.bmp|JPEG(*.JPG,*.JPEG)|*.jpg;*.jpeg|PNG(*.png)|*.png|BMP(*.bmp)|*.bmp", EFileDialogFlags::None, PictureArray))
	{
		IconImageBrush.SetResourceObject(GetLocalTexture(PictureArray[0]));
		IconFilePath = PictureArray[0];
	}
	return FReply::Handled();
}

FString UThemeInfoWindowFactory::GetPluginResourcePath(FString ResourceFileName)
{
	return IPluginManager::Get().FindPlugin(TEXT("UE4EditorCustomize"))->GetBaseDir() + FString("/Resources/") + ResourceFileName;
}

bool UThemeInfoWindowFactory::ShowModalDialog()
{
	//FSlateApplication::Get().AddWindow(UThemeInfoWindow.ToSharedRef(), true);
	FSlateApplication::Get().AddModalWindow(UThemeInfoWindow.ToSharedRef(), FSlateApplication::Get().GetActiveTopLevelWindow());
	return IsConfirmed;
}

void UThemeInfoWindowFactory::AddPreviewPicture(UTexture2D* Picture, bool IsRemoveable, const FString& FilePath/*=FString()*/)
{
	TSharedPtr<SConstraintCanvas> tmpCanvasPanel;
	FVector2D ImageSize = FVector2D(Picture->GetSizeX()*(300.f / Picture->GetSizeY()), 300.f);
	FSlateBrush* tmpCurrentBrush = new FSlateImageBrush(Picture, ImageSize);
	tmpCurrentBrush->Tiling = ESlateBrushTileType::NoTile;
	tmpImageBrush.Add(tmpCurrentBrush);
	PreviewPicBox->AddSlot().VAlign(VAlign_Fill).HAlign(HAlign_Fill).AutoWidth().Padding(FMargin(3, 3, 3, 3))
		[
			SAssignNew(tmpCanvasPanel, SConstraintCanvas)
			+ SConstraintCanvas::Slot().Anchors(FAnchors(0, 0)).Offset(FMargin(0.f, 0.f, ImageSize.X, 300.f)).Alignment(FVector2D(0, 0)).ZOrder(0)
		[
			SNew(SImage)
			.Image(tmpCurrentBrush)
		]
		];
	if (IsRemoveable)
	{
		tmpCanvasPanel->AddSlot()
			.Anchors(FAnchors(1.f, 0.f))
			.Alignment(FVector2D(1, 0))
			.Offset(FMargin(0, 0, 50.f, 50.f))
			.ZOrder(1)
			[
				SNew(SButton)
				.ButtonStyle(&RemoveImageButton)
			.OnClicked(FOnClicked::CreateLambda([tmpCanvasPanel, FilePath, this]()
				{
					tmpImagePath.Remove(FilePath);
					PreviewPicBox->RemoveSlot(tmpCanvasPanel.ToSharedRef());
					return FReply::Handled();
				}))
			];
	}
	else
	{
		tmpCanvasPanel->SetOnMouseButtonUp(FPointerEventHandler::CreateLambda([Picture](const FGeometry& Geometry, const FPointerEvent& PointerEvent)
			{
				FVector2D PicSize = FVector2D(Picture->GetSizeX(), Picture->GetSizeY());
				TSharedPtr<SWindow> PreviewWindow;
				SAssignNew(PreviewWindow, SWindow)
					.Title(LOCTEXT("ImagePreview", "Image Preview"))
					.ClientSize(PicSize*1280/Picture->GetSizeX())
					.Content()
					[
						SNew(SImage)
							.Image(new FSlateImageBrush(Picture, PicSize))
					];
				PreviewWindow->SetOnMouseDoubleClick(FPointerEventHandler::CreateLambda([PreviewWindow](const FGeometry& Geometry, const FPointerEvent& PointerEvent)
					{
						if (PreviewWindow->IsWindowMaximized())
							PreviewWindow->Restore();
						else
							PreviewWindow->Maximize();
						return FReply::Handled();
					}));
				PreviewWindow->SetAsModalWindow();
				FSlateApplication::Get().AddModalWindow(PreviewWindow.ToSharedRef(), FSlateApplication::Get().GetActiveTopLevelWindow());
				return FReply::Handled();
			}));
	}
	PreviewPicBox->Invalidate(EInvalidateWidget::LayoutAndVolatility);
	MainVerticlePanel->Invalidate(EInvalidateWidget::LayoutAndVolatility);
}

void UThemeInfoWindowFactory::ApplyUThemeInfo(FUThemeInfo_v0 UThemeInfo)
{
	NameBox->SetText(FText::FromString(UThemeInfo.Name));
	AuthorBox->SetText(FText::FromString(UThemeInfo.Author));
	SizeBox->SetText(FText::Format(FTextFormat::FromString("{0}MB"), FText::FromString(FString::SanitizeFloat((int)(UThemeInfo.Size * 100) / 100.f))));
	VersionBox->SetText(FText::Format(FTextFormat::FromString("UTheme v{0}"), UThemeInfo.Version));
	EngineVersionBox->SetText(FText::Format(FTextFormat::FromString("UE 4.{0}"),UThemeInfo.RequestEngineVersion));
	IntroBox->SetText(FText::FromString(UThemeInfo.Intro));
}

void UThemeInfoWindowFactory::SetIconImage(UTexture2D* Picture)
{
	IconImageBrush.SetResourceObject(Picture);
}

FUThemeInfo_v0 UThemeInfoWindowFactory::GetInfomation()
{
	FUThemeInfo_v0 tmpInfo;
	tmpInfo.Name = Name.ToString();
	tmpInfo.Size = Size;
	tmpInfo.Version = Version;
	tmpInfo.Author = Author.ToString();
	tmpInfo.Intro = Intro.ToString();
	tmpInfo.IconImagePath = IconFilePath;
	tmpInfo.tmpImagePath = tmpImagePath;
	return tmpInfo;
}

UTexture2D* UThemeInfoWindowFactory::GetLocalTexture(const FString &_TexPath)
{
	UTexture2D* OutTex = NULL;
	IImageWrapperModule& imageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> imageWrapper;
	FString FileExtension = FPaths::GetExtension(_TexPath);
	EPixelFormat PixelFormat = EPixelFormat::PF_R8G8B8A8;
	if (FileExtension.Equals("JPG", ESearchCase::IgnoreCase) || FileExtension.Equals("JPEG", ESearchCase::IgnoreCase))
		imageWrapper = imageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
	else if (FileExtension.Equals("PNG", ESearchCase::IgnoreCase))
		imageWrapper = imageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
	else
		imageWrapper = imageWrapperModule.CreateImageWrapper(EImageFormat::BMP);
	TArray<uint8> OutArray;
	if (FFileHelper::LoadFileToArray(OutArray, *_TexPath))
	{
		if (imageWrapper.IsValid() &&
			imageWrapper->SetCompressed(OutArray.GetData(), OutArray.Num()))
		{
			const TArray<uint8>* uncompressedRGBA = NULL;
			if (imageWrapper->GetRaw(ERGBFormat::RGBA, 8, uncompressedRGBA))
			{
				const TArray<FColor> uncompressedFColor = uint8ToFColor(*uncompressedRGBA);
				OutTex = TextureFromImage(
					imageWrapper->GetWidth(),
					imageWrapper->GetHeight(),
					uncompressedFColor,
					true, PixelFormat);
			}
		}
	}
	return OutTex;
}

UTexture2D* UThemeInfoWindowFactory::GetLocalTexture(TArray<uint8> &ProvideData, FString ImageType)
{
	UTexture2D* OutTex = NULL;
	IImageWrapperModule& imageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> imageWrapper;
	EPixelFormat PixelFormat = EPixelFormat::PF_R8G8B8A8;
	if (ImageType.Equals("JPG", ESearchCase::IgnoreCase) || ImageType.Equals("JPEG", ESearchCase::IgnoreCase))
		imageWrapper = imageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
	else if (ImageType.Equals("PNG", ESearchCase::IgnoreCase))
	{
		//PixelFormat = EPixelFormat::PF_R8G8B8A8;    
		//This Code will couse Error color of Image while import UTheme.
		//But It must be used while package.
		imageWrapper = imageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
	}
	else
		imageWrapper = imageWrapperModule.CreateImageWrapper(EImageFormat::BMP);
	if (imageWrapper.IsValid() &&
		imageWrapper->SetCompressed(ProvideData.GetData(), ProvideData.Num()))
	{
		const TArray<uint8>* uncompressedRGBA = NULL;
		if (imageWrapper->GetRaw(ERGBFormat::RGBA, 8, uncompressedRGBA))
		{
			const TArray<FColor> uncompressedFColor = uint8ToFColor(*uncompressedRGBA);
			OutTex = TextureFromImage(
				imageWrapper->GetWidth(),
				imageWrapper->GetHeight(),
				uncompressedFColor,
				true, PixelFormat);
		}
	}
	return OutTex;
}

TArray<FColor> UThemeInfoWindowFactory::uint8ToFColor(const TArray<uint8> origin)
{
	TArray<FColor> uncompressedFColor;
	uint8 auxOrigin;
	FColor auxDst;

	for (int i = 0; i < origin.Num(); i++)
	{
		auxOrigin = origin[i];
		auxDst.R = auxOrigin;
		i++;
		auxOrigin = origin[i];
		auxDst.G = auxOrigin;
		i++;
		auxOrigin = origin[i];
		auxDst.B = auxOrigin;
		i++;
		auxOrigin = origin[i];
		auxDst.A = auxOrigin;
		uncompressedFColor.Add(auxDst);
	}

	return uncompressedFColor;
}

UTexture2D* UThemeInfoWindowFactory::TextureFromImage(const int32 SrcWidth, const int32 SrcHeight, const TArray<FColor>&SrcData, const bool UseAlpha, EPixelFormat PixelFormat)
{
	UTexture2D* MyScreenshot = UTexture2D::CreateTransient(SrcWidth, SrcHeight, PixelFormat);
	uint8* MipData = static_cast<uint8*>(MyScreenshot->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE));
	uint8* DestPtr = NULL;
	const FColor* SrcPtr = NULL;
	for (int32 y = 0; y < SrcHeight; y++)
	{
		DestPtr = &MipData[(SrcHeight - 1 - y)*SrcWidth * sizeof(FColor)];
		SrcPtr = const_cast<FColor*>(&SrcData[(SrcHeight - 1 - y)*SrcWidth]);
		for (int32 x = 0; x < SrcWidth; x++)
		{
			*DestPtr++ = SrcPtr->R;
			*DestPtr++ = SrcPtr->G;
			*DestPtr++ = SrcPtr->B;
			if (UseAlpha)
			{
				*DestPtr++ = SrcPtr->A;
			}
			else
			{
				*DestPtr++ = 0xFF;
			}

			SrcPtr++;
		}
	}

	MyScreenshot->PlatformData->Mips[0].BulkData.Unlock();
	MyScreenshot->UpdateResource();
	return MyScreenshot;
}
#undef LOCTEXT_NAMESPACE
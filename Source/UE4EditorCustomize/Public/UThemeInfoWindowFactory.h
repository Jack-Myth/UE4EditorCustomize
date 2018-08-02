// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "UE4EditorCustomize.h"
#include "Widgets/SWindow.h"
#include "Brushes/SlateImageBrush.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "Widgets/Input/SEditableTextBox.h"

/**
 * 
 */
class UE4EDITORCUSTOMIZE_API UThemeInfoWindowFactory
{
	TSharedPtr<SWindow> UThemeInfoWindow;
	bool InfoEditable;
	bool IsConfirmed=false;
	TSharedPtr<SVerticalBox> MainVerticlePanel,MainInfoPanel;
	TSharedPtr<SEditableTextBox> NameBox, SizeBox, VersionBox,EngineVersionBox,AuthorBox;
	TSharedPtr<SHorizontalBox> PreviewPicBox;
	TSharedPtr<SHorizontalBox::FSlot> PreViewPicBoxLastSlot;
	TSharedPtr<SMultiLineEditableTextBox> IntroBox;
	TSharedPtr<SButton> AddPreviewImageButton;
	TArray<FString> tmpImagePath;
	TArray<FSlateBrush*> tmpImageBrush;
	FSlateBrush IconImageBrush,RemoveImageBrush;
	FButtonStyle RemoveImageButton;
	FText Name, Author, Intro;
	float Size;
	int Version;
	int EngineVersion;
	FString IconFilePath;
	void BuildSlateBrush();
	FReply OnThemeIconMouseDown(const FGeometry& Geometry, const FPointerEvent& PointerEvent);
	static UTexture2D* TextureFromImage(const int32 SrcWidth, const int32 SrcHeight, const TArray<FColor>&SrcData, const bool UseAlpha, EPixelFormat PixelFormat);
	static TArray<FColor> uint8ToFColor(const TArray<uint8> origin);
public:
	UThemeInfoWindowFactory(bool Editable);
	~UThemeInfoWindowFactory();
	static UTexture2D* GetLocalTexture(const FString &_TexPath);
	static UTexture2D* GetLocalTexture(TArray<uint8> &ProvideData,FString ImageType);
	inline bool IsEditable() { return InfoEditable; };
	FString GetPluginResourcePath(FString ResourceFileName);
	bool ShowModalDialog();
	void AddPreviewPicture(UTexture2D* Picture, bool IsRemoveable, const FString& FilePath=FString());
	void ApplyUThemeInfo(FUThemeInfo_v0 UThemeInfo);
	void SetIconImage(UTexture2D* Picture);
	FUThemeInfo_v0 GetInfomation();
};

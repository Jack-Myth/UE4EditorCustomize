// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "SettingsCustomization.h"
#include <DetailLayoutBuilder.h>
#include <DetailCategoryBuilder.h>
#include <SButton.h>
#include <ModuleManager.h>
#include <DetailWidgetRow.h>
#include <MessageDialog.h>

#define LOCTEXT_NAMESPACE "FUE4EditorCustomizeModule"

SettingsCustomization::SettingsCustomization()
{
}

SettingsCustomization::~SettingsCustomization()
{
}

void SettingsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailLyoutBuilder)
{
	IDetailCategoryBuilder& ResetToDefaultCategory= DetailLyoutBuilder.EditCategory(TEXT("UE4EditorCustomize"));
	ResetToDefaultCategory.AddCustomRow(FText::FromString("UE4EditorCustomize"))
		.WholeRowWidget
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		[
			SNew(SButton)
			.Text(LOCTEXT("ImportTheme", "Import Theme"))
			.HAlign(EHorizontalAlignment::HAlign_Center)
			.ToolTipText(LOCTEXT("ImportThemeButtonTip", "Import Theme From a file.\nBoth *.UTheme and *.Ini are Supported."))
			.OnClicked_Lambda([]()
						  {
							  FModuleManager::LoadModuleChecked<FUE4EditorCustomizeModule>("UE4EditorCustomize").ShowDialogForImport();
							  return FReply::Handled();
						  })
		]
	+ SHorizontalBox::Slot()
		[
			SNew(SButton)
			.Text(LOCTEXT("PackageTheme", "Package Current Theme"))
			.ToolTipText(LOCTEXT("PackageThemeButtonTip","Package Current Theme as a *.UTheme file.\n"
								 "A UTheme file include all Contents referenced by the ConfigFile in UE4EditorCustomize content floder. You can share it to other people.\n"
								"You can also export only a ini file. But it will not contain any Content."))
			.HAlign(EHorizontalAlignment::HAlign_Center)
			.OnClicked_Lambda([]()
						  {
							  FModuleManager::LoadModuleChecked<FUE4EditorCustomizeModule>("UE4EditorCustomize").ShowDialogForPackageTheme();
							  return FReply::Handled();
						  })
		]
		]
	+ SVerticalBox::Slot()
		[
			SNew(SButton)
			.Text(LOCTEXT("ImportBuiltInTheme", "Import Built-In Theme"))
			.HAlign(EHorizontalAlignment::HAlign_Center)
			.OnClicked_Lambda([]()
							  {
								  FModuleManager::LoadModuleChecked<FUE4EditorCustomizeModule>("UE4EditorCustomize").ImportBuiltInTheme();
								  return FReply::Handled();
							  })
		]
			+ SVerticalBox::Slot()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		[
			SNew(SButton)
			.Text(LOCTEXT("ResetEditorStyle", "Reset Editor Style"))
			.HAlign(EHorizontalAlignment::HAlign_Center)
			.ToolTipText(LOCTEXT("ResetEditorStyleButtonTip", "Reset all settings that in \"Editor Style\" Category"))
			.OnClicked_Lambda([]()
						  {
							  FModuleManager::LoadModuleChecked<FUE4EditorCustomizeModule>("UE4EditorCustomize").ResetEditorStyle();
							  FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("EditorStyleHasBeenReset", "Editor Style has been reset.Restart Editor to take effect."));
							  return FReply::Handled();
						  })
		]
	+ SHorizontalBox::Slot()
		[
			SNew(SButton)
			.Text(LOCTEXT("ResetCoreStyle", "Reset Core Style"))
			.HAlign(EHorizontalAlignment::HAlign_Center)
			.ToolTipText(LOCTEXT("ResetCoreStyleButtonTip", "Reset all settings that in \"Core Style\" Category"))
			.OnClicked_Lambda([]()
						  {
							  FModuleManager::LoadModuleChecked<FUE4EditorCustomizeModule>("UE4EditorCustomize").ResetCoreStyle();
							  FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("CoreStyleHasBeenReset", "Core Style has been reset.Restart Editor to take effect"));
							  return FReply::Handled();
						  })
		]
	+ SHorizontalBox::Slot()
		[
			SNew(SButton)
			.Text(LOCTEXT("ResetTextStyle", "Reset Text Style"))
			.ToolTipText(LOCTEXT("ResetTextStyleButtonTip", "Reset all settings that in \"Text Style\" Category"))
			.HAlign(EHorizontalAlignment::HAlign_Center)
			.OnClicked_Lambda([]()
						  {
							  FModuleManager::LoadModuleChecked<FUE4EditorCustomizeModule>("UE4EditorCustomize").ResetTextStyle();
							  FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("TextStyleHasBeenReset", "Text Style has been reset.Restart Editor to take effect"));
							  return FReply::Handled();
						  })
		]
		]
		];
}

#undef LOCTEXT_NAMESPACE
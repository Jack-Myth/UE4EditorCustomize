// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "SettingsCustomization.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "Widgets/Input/SButton.h"
#include "Modules/ModuleManager.h"
#include "DetailWidgetRow.h"
#include "Misc/MessageDialog.h"
#include <Widgets/Text/STextBlock.h>
#include <Widgets/Input/SEditableText.h>
#include <SNameComboBox.h>
#include <../Private/PropertyNode.h>

#define LOCTEXT_NAMESPACE "FUE4EditorCustomizeModule"

UE4ECSettingsCustomization::UE4ECSettingsCustomization()
{
}

UE4ECSettingsCustomization::~UE4ECSettingsCustomization()
{
}

void UE4ECSettingsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailLyoutBuilder)
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

/*void UE4ECCustomStyleEditorCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	auto& UE4ECModule = FModuleManager::LoadModuleChecked<FUE4EditorCustomizeModule>("UE4EditorCustomize");
	auto& CustomStyleCategory = DetailBuilder.EditCategory(TEXT("CustomStyle"));
	CustomStyleCategory.AddCustomRow(FText::FromString("+"))[
		SNew(SButton)
			.Text(LOCTEXT("AddCustomStyle", "Add Custom Style"))
			.OnClicked_Lambda([&CustomStyleCategory]()
				{
					TSharedPtr<SEditableText> StyleNameWidget;
					TSharedPtr<SNameComboBox> StyleTypeWidget;
					const char* StyleTypeCollection[] =
					{
						"FSlateBrush"
					};
					TArray<TSharedPtr<FName>> StyleTypeSource;
					for (const char* mStyleType: StyleTypeCollection)
					{
						StyleTypeSource.Add(MakeShareable(new FName(mStyleType)));
					}

					//Open a dialog
					SNew(SWindow)
						.Title(LOCTEXT("AddCustomStyle", "Add Custom Style"))
						.Content()[
							SNew(SVerticalBox)
								+ SVerticalBox::Slot()[
									SNew(STextBlock).Text(LOCTEXT("StyleName:", "Style Name:"))
								]
								+ SVerticalBox::Slot()[
									SAssignNew(StyleNameWidget, SEditableText)
								]
								+ SVerticalBox::Slot()[
									SNew(STextBlock).Text(LOCTEXT("StyleType:", "Style Type:"))
								]
								+ SVerticalBox::Slot()[
									SAssignNew(StyleTypeWidget,SNameComboBox)
									.OptionsSource(&StyleTypeSource)
									.InitiallySelectedItem(StyleTypeSource[0])
								]
								+ SVerticalBox::Slot()[
									SNew(SButton)
										.Text(LOCTEXT("OK","OK"))
										.OnClicked_Lambda([]()
										{

										})
								].HAlign(HAlign_Right)
						]
				})
	];

}*/

#undef LOCTEXT_NAMESPACE

// Copyright 2018 Jack Myth. All Rights Reserved.

#include "UE4EditorCustomize.h"
#include "EditorStyleSet.h"
#include "SlateStyle.h"
#include "SlateMaterialBrush.h"
#include "EditorStyleSettings.h"
#include "ModuleManager.h"
#include "EditorCustomizeSetting.h"
#include "ISettingsSection.h"
#include "Runtime/Core/Public/Misc/ConfigCacheIni.h"
#include "MessageDialog.h"
#include "../DesktopPlatform/Public/DesktopPlatformModule.h"
#include "IPluginManager.h"

#define LOCTEXT_NAMESPACE "FUE4EditorCustomizeModule"

void FUE4EditorCustomizeModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	UEditorCustomizeSetting* StyleSettings = GetMutableDefault<UEditorCustomizeSetting>();
	GetMutableDefault<UEditorStyleSettings>()->bUseGrid = StyleSettings->EditorUseGrid;
	ApplyCoreStyle(StyleSettings);
	ApplyEditorStyle(StyleSettings);
	ApplyTextStyle(StyleSettings);
	SettingS = FModuleManager::LoadModuleChecked<ISettingsModule>("Settings").RegisterSettings("Project", "Plugins",
																					TEXT("UE4EditorCustomize"), FText::FromString("UE4 EditorCustomize"),
																					FText::FromString("Setting For UE4EditorCustomize"), GetMutableDefault<UEditorCustomizeSetting>());
	SettingS->OnModified().BindRaw(this, &FUE4EditorCustomizeModule::OnSettingModified);
	SettingS->OnResetDefaults().BindLambda([=]()->bool
										   {
											   FString ConfigName = GetMutableDefault<UEditorCustomizeSetting>()->GetClass()->GetConfigName();
											   GConfig->EmptySection(*GetMutableDefault<UEditorCustomizeSetting>()->GetClass()->GetPathName(), ConfigName);
											   GConfig->Flush(false);
											   FConfigCacheIni::LoadGlobalIniFile(ConfigName, *FPaths::GetBaseFilename(ConfigName), nullptr, true);
											   GetMutableDefault<UEditorCustomizeSetting>()->ReloadConfig(nullptr, nullptr, UE4::LCPF_PropagateToInstances | UE4::LCPF_PropagateToChildDefaultObjects);
											   FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("Config has been reset.Restart Editor to take effect."));
											   return true;
										   });
}

void FUE4EditorCustomizeModule::ShutdownModule()
{
	FEditorStyle::ResetToDefault();
	FCoreStyle::ResetToDefault();
	FModuleManager::LoadModuleChecked<ISettingsModule>("Settings").UnregisterSettings("Project", "Plugins", "UE4EditorCustomize");
}

bool FUE4EditorCustomizeModule::OnSettingModified()
{
	UEditorCustomizeSetting* StyleSettings = GetMutableDefault<UEditorCustomizeSetting>();
	FString Sec = GetMutableDefault<UEditorCustomizeSetting>()->GetClass()->GetPathName();
	FString ConfigName = GetMutableDefault<UEditorCustomizeSetting>()->GetClass()->GetConfigName();
	bool IsReset=false;
	if (StyleSettings->Reset_EditorStyle)
	{
		StyleSettings->Reset_EditorStyle = false;
		GConfig->RemoveKey(*Sec, TEXT("Grap_Panel_Background"), ConfigName);
		GConfig->RemoveKey(*Sec, TEXT("Graph_Panel"), ConfigName);
		GConfig->RemoveKey(*Sec, TEXT("ToolPanel_GroupBorder"), ConfigName);
		GConfig->RemoveKey(*Sec, TEXT("ToolPanel_DarkGroupBorder"), ConfigName);
		GConfig->RemoveKey(*Sec, TEXT("SCSEditor_TreePanel"), ConfigName);
		GConfig->RemoveKey(*Sec, TEXT("SettingsEditor_CheckoutWarningBorder"), ConfigName);
		GConfig->RemoveKey(*Sec, TEXT("TableView_DarkRow"), ConfigName);
		GConfig->RemoveKey(*Sec, TEXT("DetailsView"), ConfigName);
		GConfig->RemoveKey(*Sec, TEXT("PlacementBrowser_Tab"), ConfigName);
		GConfig->RemoveKey(*Sec, TEXT("TableView_Header"), ConfigName);
		GConfig->RemoveKey(*Sec, TEXT("E_Toolbar_Background"), ConfigName);
		GConfig->RemoveKey(*Sec, TEXT("UMGEditor_Palette"), ConfigName);
		GConfig->Flush(false);
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("Editor Style has been reset.Restart Editor to take effect.")));
		return false;
	}
	if (StyleSettings->Reset_CoreStyle)
	{
		StyleSettings->Reset_CoreStyle = false;
		GConfig->RemoveKey(*Sec, TEXT("WindowStyle"), ConfigName);
		GConfig->RemoveKey(*Sec, TEXT("Docking_MajorTab"), ConfigName);
		GConfig->RemoveKey(*Sec, TEXT("Docking_Tab"), ConfigName);
		GConfig->RemoveKey(*Sec, TEXT("ToolBar_Background"), ConfigName);
		GConfig->RemoveKey(*Sec, TEXT("Menu_Background"), ConfigName);
		GConfig->Flush(false);
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("Core Style has been reset.Restart Editor to take effect")));
		return false;
	}
	if (StyleSettings->Reset_TextStyle)
	{
		StyleSettings->Reset_TextStyle = false;
		GConfig->RemoveKey(*Sec, TEXT("NormalText"), ConfigName);
		GConfig->RemoveKey(*Sec, TEXT("NormalUnderlinedText"), ConfigName);
		GConfig->RemoveKey(*Sec, TEXT("SmallText"), ConfigName);
		GConfig->RemoveKey(*Sec, TEXT("SmallUnderlinedText"), ConfigName);
		GConfig->RemoveKey(*Sec, TEXT("ToolBar_Label"), ConfigName);
		GConfig->RemoveKey(*Sec, TEXT("Docking_TabFont"), ConfigName);
		GConfig->RemoveKey(*Sec, TEXT("Menu_Label"), ConfigName);
		GConfig->RemoveKey(*Sec, TEXT("Menu_Heading"), ConfigName);
		GConfig->RemoveKey(*Sec, TEXT("PlacementBrowser_Asset_Name"), ConfigName);
		GConfig->RemoveKey(*Sec, TEXT("PlacementBrowser_Tab_Text"), ConfigName);
		GConfig->RemoveKey(*Sec, TEXT("PropertyWindow"), ConfigName);
		GConfig->RemoveKey(*Sec, TEXT("Hyperlink"), ConfigName);
		GConfig->RemoveKey(*Sec, TEXT("InlineEditableTextBlockStyle"), ConfigName);
		GConfig->RemoveKey(*Sec, TEXT("DetailsView_CategoryFontStyle"), ConfigName);
		GConfig->RemoveKey(*Sec, TEXT("SettingsEditor_CatgoryAndSectionFont"), ConfigName);
		GConfig->RemoveKey(*Sec, TEXT("ContentBrowserFont"), ConfigName);
		GConfig->Flush(false);
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("Text Style has been reset.Restart Editor to take effect")));
		return false;
	}
	if (StyleSettings->ShouldImportSetting)
	{
		StyleSettings->ShouldImportSetting = false;
		TArray<FString> OutFiles;
		if (FDesktopPlatformModule::Get()->OpenFileDialog(NULL, "Open Config", FPaths::GetPath(GEditorPerProjectIni), TEXT(""), TEXT("Config files (*.ini)|*.ini"), EFileDialogFlags::None, OutFiles))
		{
			if (SettingS->Import(OutFiles[0])&&SettingS->Save())
			{
				FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("Import Success.")));
			}
			else
			{
				FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("Import Failed.")));
			}
		}
		return true;
	}
	if (StyleSettings->ShouldImportBuildinConfig)
	{
		StyleSettings->ShouldImportBuildinConfig = false;
		if (SettingS->Import(IPluginManager::Get().FindPlugin(TEXT("UE4EditorCustomize"))->GetBaseDir()+FString("/Config/UE4EditorCustomize.ini")) && SettingS->Save())
		{
			FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("Import Success.")));
		}
		else
		{
			FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("Import Failed.")));
		}
		return true;
	}
	GetMutableDefault<UEditorStyleSettings>()->bUseGrid = StyleSettings->EditorUseGrid;
	(FCheckBoxStyle&)FEditorStyle::GetWidgetStyle<FCheckBoxStyle>("PlacementBrowser.Tab") = StyleSettings->PlacementBrowser_Tab;
	(FCheckBoxStyle&)FEditorStyle::GetWidgetStyle<FCheckBoxStyle>("EditorModesToolbar.ToggleButton") = StyleSettings->EditorModesToolbar_ToggleButton;
	(FTableRowStyle&)FEditorStyle::GetWidgetStyle<FTableRowStyle>("TableView.DarkRow") = StyleSettings->TableView_DarkRow;
	(FTableRowStyle&)FEditorStyle::GetWidgetStyle<FTableRowStyle>("UMGEditor.PaletteHeader") = StyleSettings->UMGEditor_Palette.UMGEditor_PaletteHeader;
	(FTableRowStyle&)FEditorStyle::GetWidgetStyle<FTableRowStyle>("UMGEditor.PaletteItem") = StyleSettings->UMGEditor_Palette.UMGEditor_PaletteItem;
	(FLinearColor&)FEditorStyle::GetColor("Graph.Panel.GridLineColor") = StyleSettings->Graph_Panel.GridLineColor;
	(FLinearColor&)FEditorStyle::GetColor("Graph.Panel.GridRuleColor") = StyleSettings->Graph_Panel.GridRuleColor;
	(FLinearColor&)FEditorStyle::GetColor("Graph.Panel.GridCenterColor") = StyleSettings->Graph_Panel.GridCenterColor;
	ApplyCoreStyle(StyleSettings);
	ApplyTextStyle(StyleSettings);
	return true;
}

void FUE4EditorCustomizeModule::ApplyEditorStyle(UEditorCustomizeSetting* StyleSettings)
{
	FSlateStyleSet* EditorStyles = (FSlateStyleSet*)&FEditorStyle::Get();
	EditorStyles->Set("Graph.Panel.SolidBackground", &StyleSettings->Grap_Panel_Background);
	EditorStyles->Set("ToolPanel.GroupBorder", &StyleSettings->ToolPanel_GroupBorder);
	EditorStyles->Set("ToolPanel.DarkGroupBorder", &StyleSettings->ToolPanel_DarkGroupBorder);
	EditorStyles->Set("SCSEditor.TreePanel", &StyleSettings->SCSEditor_TreePanel);
	EditorStyles->Set("SettingsEditor.CheckoutWarningBorder", &StyleSettings->SettingsEditor_CheckoutWarningBorder);
	EditorStyles->Set("DetailsView.CategoryTop_Hovered", &StyleSettings->DetailsView.CategoryTop_Hovered);
	EditorStyles->Set("DetailsView.CategoryTop", &StyleSettings->DetailsView.CategoryTop);
	EditorStyles->Set("DetailsView.CategoryMiddle_Hovered", &StyleSettings->DetailsView.CategoryMiddle_Hovered);
	EditorStyles->Set("DetailsView.CategoryMiddle", &StyleSettings->DetailsView.CategoryMiddle_Hovered);
	EditorStyles->Set("DetailsView.CategoryMiddle_Highlighted", &StyleSettings->DetailsView.CategoryMiddle_Highlighted);
	EditorStyles->Set("DetailsView.CollapsedCategory_Hovered", &StyleSettings->DetailsView.CollapsedCategory_Hovered);
	EditorStyles->Set("DetailsView.CollapsedCategory", &StyleSettings->DetailsView.CollapsedCategory);
	EditorStyles->Set("DetailsView.CategoryBottom", &StyleSettings->DetailsView.CategoryBottom);
	EditorStyles->Set("DetailsView.AdvancedDropdownBorder", &StyleSettings->DetailsView.AdvancedDropdownBorder);
	EditorStyles->Set(TEXT("Toolbar.Background"), &StyleSettings->E_Toolbar_Background);
	(FCheckBoxStyle&)FEditorStyle::GetWidgetStyle<FCheckBoxStyle>("PlacementBrowser.Tab") = StyleSettings->PlacementBrowser_Tab;
	(FCheckBoxStyle&)FEditorStyle::GetWidgetStyle<FCheckBoxStyle>("EditorModesToolbar.ToggleButton") = StyleSettings->EditorModesToolbar_ToggleButton;
	(FTableRowStyle&)FEditorStyle::GetWidgetStyle<FTableRowStyle>("TableView.DarkRow") = StyleSettings->TableView_DarkRow;
	(FTableRowStyle&)FEditorStyle::GetWidgetStyle<FTableRowStyle>("UMGEditor.PaletteHeader") = StyleSettings->UMGEditor_Palette.UMGEditor_PaletteHeader;
	(FTableRowStyle&)FEditorStyle::GetWidgetStyle<FTableRowStyle>("UMGEditor.PaletteItem") = StyleSettings->UMGEditor_Palette.UMGEditor_PaletteItem;
	(FLinearColor&)FEditorStyle::GetColor("Graph.Panel.GridLineColor") = StyleSettings->Graph_Panel.GridLineColor;
	(FLinearColor&)FEditorStyle::GetColor("Graph.Panel.GridRuleColor") = StyleSettings->Graph_Panel.GridRuleColor;
	(FLinearColor&)FEditorStyle::GetColor("Graph.Panel.GridCenterColor") = StyleSettings->Graph_Panel.GridCenterColor;

}

void FUE4EditorCustomizeModule::ApplyCoreStyle(class UEditorCustomizeSetting* StyleSettings)
{
	auto& CoreStyles = FCoreStyle::Get();
	(FWindowStyle&)CoreStyles.GetWidgetStyle<FWindowStyle>("Window") = StyleSettings->WindowStyle;
	(FDockTabStyle&)CoreStyles.GetWidgetStyle<FDockTabStyle>("Docking.MajorTab") = StyleSettings->Docking_MajorTab;
	(FDockTabStyle&)CoreStyles.GetWidgetStyle<FDockTabStyle>("Docking.Tab") = StyleSettings->Docking_Tab;
	(FHeaderRowStyle&)CoreStyles.GetWidgetStyle<FHeaderRowStyle>("TableView.Header")=StyleSettings->TableView_Header;
	*(FSlateBrush*)CoreStyles.GetBrush("ToolBar.Background") = StyleSettings->ToolBar_Background;
	*(FSlateBrush*)CoreStyles.GetBrush("Menu.Background") = StyleSettings->Menu_Background;
}

void FUE4EditorCustomizeModule::ApplyTextStyle(class UEditorCustomizeSetting* StyleSettings)
{
	(FTextBlockStyle&)FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText") = StyleSettings->NormalText;
	(FTextBlockStyle&)FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalUnderlinedText") = StyleSettings->NormalUnderlinedText;
	(FTextBlockStyle&)FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("SmallText") = StyleSettings->SmallText;
	(FTextBlockStyle&)FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("SmallUnderlinedText") = StyleSettings->SmallUnderlinedText;
	(FTextBlockStyle&)FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("ToolBar.Label") = StyleSettings->ToolBar_Label;
	(FTextBlockStyle&)FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("Docking.TabFont") = StyleSettings->Docking_TabFont;
	(FTextBlockStyle&)FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("Menu.Label") = StyleSettings->Menu_Label;
	(FTextBlockStyle&)FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("Menu.Heading") = StyleSettings->Menu_Heading;
	(FTextBlockStyle&)FEditorStyle::GetWidgetStyle<FTextBlockStyle>("PlacementBrowser.Asset.Name") = StyleSettings->PlacementBrowser_Asset_Name;
	(FTextBlockStyle&)FEditorStyle::GetWidgetStyle<FTextBlockStyle>("PlacementBrowser.Tab.Text") = StyleSettings->PlacementBrowser_Tab_Text;
	((FSlateStyleSet&)FEditorStyle::Get()).Set("PropertyWindow.NormalFont", StyleSettings->PropertyWindow.NormalFont);
	((FSlateStyleSet&)FEditorStyle::Get()).Set("PropertyWindow.BoldFont", StyleSettings->PropertyWindow.BoldFont);
	((FSlateStyleSet&)FEditorStyle::Get()).Set("PropertyWindow.ItalicFont", StyleSettings->PropertyWindow.ItalicFont);
	((FSlateStyleSet&)FEditorStyle::Get()).Set("DetailsView.CategoryFontStyle", StyleSettings->DetailsView_CategoryFontStyle);
	(FHyperlinkStyle&)FCoreStyle::Get().GetWidgetStyle<FHyperlinkStyle>("Hyperlink") = StyleSettings->Hyperlink;
	(FInlineEditableTextBlockStyle&)FCoreStyle::Get().GetWidgetStyle<FInlineEditableTextBlockStyle>("InlineEditableTextBlockStyle") = StyleSettings->InlineEditableTextBlockStyle;
	((FSlateStyleSet&)FEditorStyle::Get()).Set("SettingsEditor.CatgoryAndSectionFont", StyleSettings->SettingsEditor_CatgoryAndSectionFont);
	((FSlateStyleSet&)FEditorStyle::Get()).Set("ContentBrowser.AssetTileViewNameFont", StyleSettings->ContentBrowserFont.AssetTileViewNameFont);
	((FSlateStyleSet&)FEditorStyle::Get()).Set("ContentBrowser.AssetTileViewNameFontSmall", StyleSettings->ContentBrowserFont.AssetTileViewNameFontSmall);
	((FSlateStyleSet&)FEditorStyle::Get()).Set("ContentBrowser.AssetTileViewNameFontVerySmall", StyleSettings->ContentBrowserFont.AssetTileViewNameFontVerySmall);
	((FSlateStyleSet&)FEditorStyle::Get()).Set("ContentBrowser.SourceTreeItemFont ", StyleSettings->ContentBrowserFont.SourceTreeItemFont);
	((FSlateStyleSet&)FEditorStyle::Get()).Set("ContentBrowser.AssetTileViewNameFont", StyleSettings->ContentBrowserFont.AssetTileViewNameFont);
	((FSlateStyleSet&)FEditorStyle::Get()).Set("ContentBrowser.SourceTreeItemFont", StyleSettings->ContentBrowserFont.SourceTreeItemFont);
	((FSlateStyleSet&)FEditorStyle::Get()).Set("ContentBrowser.SourceTreeRootItemFont", StyleSettings->ContentBrowserFont.SourceTreeRootItemFont);
	(FTextBlockStyle&)FEditorStyle::GetWidgetStyle<FTextBlockStyle>("ContentBrowser.PathText") = StyleSettings->ContentBrowserFont.PathText;
	(FTextBlockStyle&)FEditorStyle::GetWidgetStyle<FTextBlockStyle>("ContentBrowser.TopBar.Font") = StyleSettings->ContentBrowserFont.TopBar_Font;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUE4EditorCustomizeModule, UE4EditorCustomize)
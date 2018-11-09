// Copyright 2018 Jack Myth. All Rights Reserved.

#include "EditorCustomizeSetting.h"
#include "SlateMaterialBrush.h"
#include "Materials/MaterialInterface.h"
#include "EditorStyleSet.h"
#include "Classes/EditorStyleSettings.h"

UEditorCustomizeSetting::UEditorCustomizeSetting()
{
	InitEditorStyle();
	InitCoreStyle();
	InitTextStyle();
	EditorUseGrid = GetDefault<UEditorStyleSettings>()->bUseGrid;
}

void UEditorCustomizeSetting::InitEditorStyle()
{
	Grap_Panel_Background = *FEditorStyle::GetBrush("Graph.Panel.SolidBackground");
	Graph_Panel.GridLineColor = FEditorStyle::GetColor("Graph.Panel.GridLineColor");
	Graph_Panel.GridRuleColor = FEditorStyle::GetColor("Graph.Panel.GridRuleColor");
	Graph_Panel.GridCenterColor = FEditorStyle::GetColor("Graph.Panel.GridCenterColor");
	ToolPanel_GroupBorder = *FEditorStyle::GetBrush("ToolPanel.GroupBorder");
	ToolPanel_DarkGroupBorder = *FEditorStyle::GetBrush("ToolPanel.DarkGroupBorder");
	SCSEditor_TreePanel = *FEditorStyle::GetBrush("SCSEditor.TreePanel");
	SettingsEditor_CheckoutWarningBorder = *FEditorStyle::GetBrush("SettingsEditor.CheckoutWarningBorder");
	TableView_DarkRow = FEditorStyle::GetWidgetStyle<FTableRowStyle>("TableView.DarkRow");
	DetailsView.CategoryTop_Hovered = *FEditorStyle::GetBrush("DetailsView.CategoryTop_Hovered");
	DetailsView.CategoryTop = *FEditorStyle::GetBrush("DetailsView.CategoryTop");
	DetailsView.CategoryMiddle = *FEditorStyle::GetBrush("DetailsView.CategoryMiddle");
	DetailsView.CategoryMiddle_Hovered = *FEditorStyle::GetBrush("DetailsView.CategoryMiddle_Hovered");
	DetailsView.CategoryMiddle_Highlighted = *FEditorStyle::GetBrush("DetailsView.CategoryMiddle_Highlighted");
	DetailsView.CollapsedCategory_Hovered = *FEditorStyle::GetBrush("DetailsView.CollapsedCategory_Hovered");
	DetailsView.CollapsedCategory = *FEditorStyle::GetBrush("DetailsView.CollapsedCategory");
	DetailsView.CategoryBottom = *FEditorStyle::GetBrush("DetailsView.CategoryBottom");
	DetailsView.AdvancedDropdownBorder = *FEditorStyle::GetBrush("DetailsView.AdvancedDropdownBorder");
	E_Toolbar_Background = *FEditorStyle::GetBrush(TEXT("Toolbar.Background"));
	PlacementBrowser_Tab = FEditorStyle::GetWidgetStyle<FCheckBoxStyle>("PlacementBrowser.Tab");
	EditorModesToolbar_ToggleButton = FEditorStyle::GetWidgetStyle<FCheckBoxStyle>("EditorModesToolbar.ToggleButton");
	UMGEditor_Palette.UMGEditor_PaletteHeader = FEditorStyle::GetWidgetStyle<FTableRowStyle>("UMGEditor.PaletteHeader");
	UMGEditor_Palette.UMGEditor_PaletteItem = FEditorStyle::GetWidgetStyle<FTableRowStyle>("UMGEditor.PaletteItem");
	Docking_Tab_ContentAreaBrush = *FEditorStyle::GetBrush("Docking.Tab.ContentAreaBrush");
}

void UEditorCustomizeSetting::InitCoreStyle()
{
	WindowStyle = FCoreStyle::Get().GetWidgetStyle<FWindowStyle>("Window");
	Docking_MajorTab = FCoreStyle::Get().GetWidgetStyle<FDockTabStyle>("Docking.MajorTab");
	Docking_Tab = FCoreStyle::Get().GetWidgetStyle<FDockTabStyle>("Docking.Tab");
	TableView_Header = FCoreStyle::Get().GetWidgetStyle<FHeaderRowStyle>("TableView.Header");
	ToolBar_Background = *FCoreStyle::Get().GetBrush("ToolBar.Background");
	Menu_Background = *FCoreStyle::Get().GetBrush("Menu.Background");
}

void UEditorCustomizeSetting::InitTextStyle()
{
	NormalText = FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText");
	NormalUnderlinedText = FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalUnderlinedText");
	SmallText = FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("SmallText");
	SmallUnderlinedText = FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("SmallUnderlinedText");
	ToolBar_Label = FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("ToolBar.Label");
	Docking_TabFont = FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("Docking.TabFont");
	Menu_Label = FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("Menu.Label");
	Menu_Heading = FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("Menu.Heading");
	PlacementBrowser_Asset_Name = FEditorStyle::GetWidgetStyle<FTextBlockStyle>("PlacementBrowser.Asset.Name");
	PlacementBrowser_Tab_Text = FEditorStyle::GetWidgetStyle<FTextBlockStyle>("PlacementBrowser.Tab.Text");
	PropertyWindow.NormalFont = FEditorStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont"));
	PropertyWindow.BoldFont = FEditorStyle::GetFontStyle(TEXT("PropertyWindow.BoldFont"));
	PropertyWindow.ItalicFont = FEditorStyle::GetFontStyle(TEXT("PropertyWindow.ItalicFont"));
	DetailsView_CategoryFontStyle = FEditorStyle::GetFontStyle("DetailsView.CategoryFontStyle");
	Hyperlink = FCoreStyle::Get().GetWidgetStyle< FHyperlinkStyle >("Hyperlink");
	InlineEditableTextBlockStyle = FCoreStyle::Get().GetWidgetStyle< FInlineEditableTextBlockStyle >("InlineEditableTextBlockStyle");
	SettingsEditor_CatgoryAndSectionFont = FEditorStyle::GetFontStyle("SettingsEditor.CatgoryAndSectionFont");
	ContentBrowserFont.AssetTileViewNameFont = FEditorStyle::GetFontStyle("ContentBrowser.AssetTileViewNameFont");
	ContentBrowserFont.AssetTileViewNameFontSmall = FEditorStyle::GetFontStyle("ContentBrowser.AssetTileViewNameFontSmall");
	ContentBrowserFont.AssetTileViewNameFontVerySmall = FEditorStyle::GetFontStyle("ContentBrowser.AssetTileViewNameFontVerySmall");
	ContentBrowserFont.SourceTreeItemFont = FEditorStyle::GetFontStyle("ContentBrowser.AssetTileViewNameFont");
	ContentBrowserFont.SourceTreeRootItemFont = FEditorStyle::GetFontStyle("ContentBrowser.AssetTileViewNameFont");
	ContentBrowserFont.PathText = FEditorStyle::GetWidgetStyle<FTextBlockStyle>("ContentBrowser.PathText");
	ContentBrowserFont.TopBar_Font = FEditorStyle::GetWidgetStyle<FTextBlockStyle>("ContentBrowser.TopBar.Font");
}

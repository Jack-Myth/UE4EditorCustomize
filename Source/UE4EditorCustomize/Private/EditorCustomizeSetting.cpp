// Copyright 2018 Jack Myth. All Rights Reserved.

#include "EditorCustomizeSetting.h"
#include "SlateMaterialBrush.h"
#include "Materials/MaterialInterface.h"
#include "EditorStyleSet.h"
#include "Classes/EditorStyleSettings.h"
#include <Styling/SlateTypes.h>
#include <Styling/SlateStyle.h>

UEditorCustomizeSetting::UEditorCustomizeSetting()
{
	InitEditorStyle();
	InitCoreStyle();
	InitTextStyle();
	//InitCustomStyle((FSlateStyleSet*)&FEditorStyle::Get(), CustomStyleEditor);
	//InitCustomStyle((FSlateStyleSet*)&FCoreStyle::Get(), CustomStyleCore);
	EditorUseGrid = GetDefault<UEditorStyleSettings>()->bUseGrid;
}

void UEditorCustomizeSetting::PostProcessCustomStyle(UScriptStruct* StyleStruct,void* StructPtr)
{
	//Property need to be processed
	//such as FSlateColor
	//SlateColor with linked flag will cause some bug while save and load.
	//It will crash the editor.
	for (UProperty* CurProperty = StyleStruct->PropertyLink;CurProperty;CurProperty=CurProperty->PropertyLinkNext)
	{
		//Check if it's a FSlateColor
		FName CppType = *CurProperty->GetCPPType();
		if (CppType =="FSlateColor")
		{
			//Unlink this color.
			auto* pColor = Cast<UStructProperty>(CurProperty)->ContainerPtrToValuePtr<FSlateColor>(StructPtr);
			if (pColor)
				pColor->Unlink();
		}
		else if (auto* StructProperty=Cast<UStructProperty>(CurProperty))
		{
			PostProcessCustomStyle(StructProperty->Struct, StructProperty->ContainerPtrToValuePtr<void>(StructPtr));
		}
	}
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
	ContentBrowser_TopBar_GroupBorder = *FEditorStyle::GetBrush("ContentBrowser.TopBar.GroupBorder");
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

void UEditorCustomizeSetting::InitCustomStyle(FSlateStyleSet* SlateStyleSet, FUE4ECCustomStyle& CustomStyle)
{
	//SlateBrush is already inited outside.
	for (auto& curFontInfo : CustomStyle.SlateFontInfo)
		curFontInfo.Value = FEditorStyle::GetFontStyle(curFontInfo.Key);
	int CurCustomStyleCount = 0;
#define CHECK_STYLE_VALID(STYLE_NAME) for (auto& curStylePair : CustomStyle.STYLE_NAME)\
			if (SlateStyleSet->HasWidgetStyle<F##STYLE_NAME>(curStylePair.Key))\
				CurCustomStyleCount++;
	CHECK_STYLE_VALID(ButtonStyle);
	CHECK_STYLE_VALID(ComboBoxStyle);
	CHECK_STYLE_VALID(ComboButtonStyle);
	CHECK_STYLE_VALID(DockTabStyle);
	CHECK_STYLE_VALID(EditableTextBoxStyle);
	CHECK_STYLE_VALID(EditableTextStyle);
	CHECK_STYLE_VALID(ExpandableAreaStyle);
	CHECK_STYLE_VALID(HeaderRowStyle);
	CHECK_STYLE_VALID(InlineEditableTextBlockStyle);
	CHECK_STYLE_VALID(InlineTextImageStyle);
	CHECK_STYLE_VALID(ProgressBarStyle);
	CHECK_STYLE_VALID(ScrollBarStyle);
	CHECK_STYLE_VALID(ScrollBorderStyle);
	CHECK_STYLE_VALID(ScrollBoxStyle);
	CHECK_STYLE_VALID(SearchBoxStyle);
	CHECK_STYLE_VALID(SliderStyle);
	CHECK_STYLE_VALID(SpinBoxStyle);
	CHECK_STYLE_VALID(SplitterStyle);
	CHECK_STYLE_VALID(TableColumnHeaderStyle);
	CHECK_STYLE_VALID(TableRowStyle);
	CHECK_STYLE_VALID(TextBlockStyle);
	CHECK_STYLE_VALID(VolumeControlStyle);
	CHECK_STYLE_VALID(WindowStyle);
#undef CHECK_STYLE_VALID
	if (CurCustomStyleCount > CustomStyle.CustomStyleCount)
	{
		//User add a new style
#define INIT_STYLE(STYLE_NAME) for (auto& curPair : CustomStyle.STYLE_NAME)\
	if (SlateStyleSet->HasWidgetStyle<F##STYLE_NAME>(curPair.Key)){\
		curPair.Value = SlateStyleSet->GetWidgetStyle<F##STYLE_NAME>(curPair.Key);\
		PostProcessCustomStyle(curPair.Value.StaticStruct(),&curPair.Value);}

		INIT_STYLE(ButtonStyle);
		INIT_STYLE(ComboBoxStyle);
		INIT_STYLE(ComboButtonStyle);
		INIT_STYLE(DockTabStyle);
		INIT_STYLE(EditableTextBoxStyle);
		INIT_STYLE(EditableTextStyle);
		INIT_STYLE(ExpandableAreaStyle);
		INIT_STYLE(HeaderRowStyle);
		INIT_STYLE(InlineEditableTextBlockStyle);
		INIT_STYLE(InlineTextImageStyle);
		INIT_STYLE(ProgressBarStyle);
		INIT_STYLE(ScrollBarStyle);
		INIT_STYLE(ScrollBorderStyle);
		INIT_STYLE(ScrollBoxStyle);
		INIT_STYLE(SearchBoxStyle);
		INIT_STYLE(SliderStyle);
		INIT_STYLE(SpinBoxStyle);
		INIT_STYLE(SplitterStyle);
		INIT_STYLE(TableColumnHeaderStyle);
		INIT_STYLE(TableRowStyle);
		INIT_STYLE(TextBlockStyle);
		INIT_STYLE(VolumeControlStyle);
		INIT_STYLE(WindowStyle);
#undef INIT_STYLE
	}
	CustomStyle.CustomStyleCount = CurCustomStyleCount;
}

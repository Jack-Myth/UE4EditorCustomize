// Copyright 2018 Jack Myth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Styling/SlateTypes.h"
#include "EditorCustomizeSetting.generated.h"

USTRUCT()
struct FEditorCustomizeDetailsView
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere,Category="DetailsViewStyle")
		FSlateBrush CategoryTop_Hovered;

	UPROPERTY(EditAnywhere, Category = "DetailsViewStyle")
		FSlateBrush CategoryTop;

	UPROPERTY(EditAnywhere, Category = "DetailsViewStyle")
		FSlateBrush CategoryMiddle_Highlighted;

	UPROPERTY(EditAnywhere, Category = "DetailsViewStyle")
		FSlateBrush CategoryMiddle_Hovered;

	UPROPERTY(EditAnywhere, Category = "DetailsViewStyle")
		FSlateBrush CategoryMiddle;

	UPROPERTY(EditAnywhere, Category = "DetailsViewStyle")
		FSlateBrush CollapsedCategory_Hovered;

	UPROPERTY(EditAnywhere, Category = "DetailsViewStyle")
		FSlateBrush CollapsedCategory;

	UPROPERTY(EditAnywhere, Category = "DetailsViewStyle")
		FSlateBrush CategoryBottom;

	UPROPERTY(EditAnywhere, Category = "DetailsViewStyle")
		FSlateBrush AdvancedDropdownBorder;
};

USTRUCT()
struct FPropertyWindow
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = "PropertyWindow")
		FSlateFontInfo NormalFont;

	UPROPERTY(EditAnywhere, Category = "PropertyWindow")
		FSlateFontInfo BoldFont;

	UPROPERTY(EditAnywhere, Category = "PropertyWindow")
		FSlateFontInfo ItalicFont;
};

USTRUCT()
struct FContentBrowserFont
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = "ContentBrowser")
		FSlateFontInfo SourceTreeRootItemFont;

	UPROPERTY(EditAnywhere, Category = "ContentBrowser")
		FSlateFontInfo SourceTreeItemFont;

	UPROPERTY(EditAnywhere, Category = "ContentBrowser")
		FSlateFontInfo AssetTileViewNameFontVerySmall;

	UPROPERTY(EditAnywhere, Category = "ContentBrowser")
		FSlateFontInfo AssetTileViewNameFontSmall;

	UPROPERTY(EditAnywhere, Category = "ContentBrowser")
		FSlateFontInfo AssetTileViewNameFont;

	UPROPERTY(EditAnywhere, Category = "ContentBrowser")
		FTextBlockStyle TopBar_Font;

	UPROPERTY(EditAnywhere, Category = "ContentBrowser")
		FTextBlockStyle PathText;
};

USTRUCT()
struct FGraphPanelStyle
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = "GraphPanelStyle")
		FLinearColor GridLineColor;

	UPROPERTY(EditAnywhere, Category = "GraphPanelStyle")
		FLinearColor GridRuleColor;

	UPROPERTY(EditAnywhere, Category = "GraphPanelStyle")
		FLinearColor GridCenterColor;
};

USTRUCT()
struct FUMGEditorPalette
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = "UMGEditorPalette", meta = (DisplayName = "UMGEditor.PaletteHeader"))
		FTableRowStyle UMGEditor_PaletteHeader;

	UPROPERTY(EditAnywhere, Category = "UMGEditorPalette", meta = (DisplayName = "UMGEditor.PaletteItem"))
		FTableRowStyle UMGEditor_PaletteItem;
};

USTRUCT()
struct FUE4ECCustomStyle
{
	GENERATED_USTRUCT_BODY()

	//////////////////////////////////////////////////////////////////////////
	////////////////////////Internal use only////////////////////////////////
	UPROPERTY(Config)
		int CustomStyleCount = 0;
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere,Category="CustomStyle", meta = (DisplayName = "FSlateBrush"))
		TMap<FName, FSlateBrush> SlateBrush;

	UPROPERTY(EditAnywhere,Category="CustomStyle",meta=(DisplayName="SlateFontInfo"))
		TMap<FName, FSlateFontInfo> SlateFontInfo;

	UPROPERTY(EditAnywhere,Category="CustomStyle", meta = (DisplayName = "FTextBlockStyle"))
		TMap<FName, FTextBlockStyle> TextBlockStyle;

	UPROPERTY(EditAnywhere,Category="CustomStyle", meta = (DisplayName = "FTextBlockStyle"))
		TMap<FName, FButtonStyle> ButtonStyle;

	UPROPERTY(EditAnywhere,Category="CustomStyle", meta = (DisplayName = "FComboButtonStyle"))
		TMap<FName, FComboButtonStyle> ComboButtonStyle;

	UPROPERTY(EditAnywhere,Category="CustomStyle", meta = (DisplayName = "FComboBoxStyle"))
		TMap<FName, FComboBoxStyle> ComboBoxStyle;

	UPROPERTY(EditAnywhere,Category="CustomStyle", meta = (DisplayName = "FEditableTextStyle"))
		TMap<FName, FEditableTextStyle> EditableTextStyle;

	UPROPERTY(EditAnywhere,Category="CustomStyle", meta = (DisplayName = "FScrollBarStyle"))
		TMap<FName, FScrollBarStyle> ScrollBarStyle;

	UPROPERTY(EditAnywhere,Category="CustomStyle", meta = (DisplayName = "FEditableTextBoxStyle"))
		TMap<FName, FEditableTextBoxStyle> EditableTextBoxStyle;

	UPROPERTY(EditAnywhere,Category="CustomStyle", meta = (DisplayName = "FInlineEditableTextBlockStyle"))
		TMap<FName, FInlineEditableTextBlockStyle> InlineEditableTextBlockStyle;

	UPROPERTY(EditAnywhere,Category="CustomStyle", meta = (DisplayName = "FProgressBarStyle"))
		TMap<FName, FProgressBarStyle> ProgressBarStyle;

	UPROPERTY(EditAnywhere,Category="CustomStyle", meta = (DisplayName = "FExpandableAreaStyle"))
		TMap<FName, FExpandableAreaStyle> ExpandableAreaStyle;

	UPROPERTY(EditAnywhere,Category="CustomStyle", meta = (DisplayName = "FSearchBoxStyle"))
		TMap<FName, FSearchBoxStyle> SearchBoxStyle;

	UPROPERTY(EditAnywhere,Category="CustomStyle", meta = (DisplayName = "FSliderStyle"))
		TMap<FName, FSliderStyle> SliderStyle;

	UPROPERTY(EditAnywhere,Category="CustomStyle", meta = (DisplayName = "FVolumeControlStyle"))
		TMap<FName, FVolumeControlStyle> VolumeControlStyle;

	UPROPERTY(EditAnywhere,Category="CustomStyle", meta = (DisplayName = "FInlineTextImageStyle"))
		TMap<FName, FInlineTextImageStyle> InlineTextImageStyle;

	UPROPERTY(EditAnywhere,Category="CustomStyle", meta = (DisplayName = "FSpinBoxStyle"))
		TMap<FName, FSpinBoxStyle> SpinBoxStyle;

	UPROPERTY(EditAnywhere,Category="CustomStyle", meta = (DisplayName = "FSplitterStyle"))
		TMap<FName, FSplitterStyle> SplitterStyle;

	UPROPERTY(EditAnywhere,Category="CustomStyle", meta = (DisplayName = "FTableRowStyle"))
		TMap<FName, FTableRowStyle> TableRowStyle;

	UPROPERTY(EditAnywhere,Category="CustomStyle", meta = (DisplayName = "FTableColumnHeaderStyle"))
		TMap<FName, FTableColumnHeaderStyle> TableColumnHeaderStyle;

	UPROPERTY(EditAnywhere,Category="CustomStyle", meta = (DisplayName = "FHeaderRowStyle"))
		TMap<FName, FHeaderRowStyle> HeaderRowStyle;

	UPROPERTY(EditAnywhere,Category="CustomStyle", meta = (DisplayName = "FDockTabStyle"))
		TMap<FName, FDockTabStyle> DockTabStyle;

	UPROPERTY(EditAnywhere,Category="CustomStyle", meta = (DisplayName = "FScrollBoxStyle"))
		TMap<FName, FScrollBoxStyle> ScrollBoxStyle;

	UPROPERTY(EditAnywhere,Category="CustomStyle", meta = (DisplayName = "FScrollBorderStyle"))
		TMap<FName, FScrollBorderStyle> ScrollBorderStyle;

	UPROPERTY(EditAnywhere,Category="CustomStyle", meta = (DisplayName = "FWindowStyle"))
		TMap<FName, FWindowStyle> WindowStyle;

};

/**
 * 
 */
UCLASS(config = Editor)
class UEditorCustomizeSetting : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(Config, EditAnywhere, Category = "Editor Style", meta = (DisplayName = "Editor.UseGrid"))
		bool EditorUseGrid;

	UPROPERTY(Config,EditAnywhere,Category="Editor Style", meta = (DisplayName = "Graph.Panel.SolidBackground"))
		FSlateBrush Grap_Panel_Background;

	UPROPERTY(Config, EditAnywhere, Category = "Editor Style", meta = (DisplayName = "Graph.Panel"))
		FGraphPanelStyle Graph_Panel;

	UPROPERTY(Config, EditAnywhere, Category = "Editor Style", meta = (DisplayName = "ToolPanel.GroupBorder"))
		FSlateBrush ToolPanel_GroupBorder;

	UPROPERTY(Config, EditAnywhere, Category = "Editor Style", meta = (DisplayName = "ToolPanel.DarkGroupBorder"))
		FSlateBrush ToolPanel_DarkGroupBorder;

	UPROPERTY(Config, EditAnywhere, Category = "Editor Style", meta = (DisplayName = "SCSEditor.TreePanel"))
		FSlateBrush SCSEditor_TreePanel;

	UPROPERTY(Config, EditAnywhere, Category = "Editor Style", meta = (DisplayName = "SettingsEditor.CheckoutWarningBorder"))
		FSlateBrush SettingsEditor_CheckoutWarningBorder;

	UPROPERTY(Config, EditAnywhere, Category = "Editor Style", meta = (DisplayName = "Docking.Tab.ContentAreaBrush"))
		FSlateBrush Docking_Tab_ContentAreaBrush;

	UPROPERTY(Config,EditAnywhere,Category="Editor Style",meta = (DisplayName="ContentBrowser.TopBar.GroupBorder"))
		FSlateBrush ContentBrowser_TopBar_GroupBorder;

	UPROPERTY(Config, EditAnywhere, Category = "Editor Style", meta = (DisplayName = "TableView.DarkRow"))
		FTableRowStyle TableView_DarkRow;

	UPROPERTY(Config, EditAnywhere, Category = "Editor Style", meta = (DisplayName = "DetailsView"))
		FEditorCustomizeDetailsView DetailsView;

	UPROPERTY(Config, EditAnywhere, Category = "Editor Style", meta = (DisplayName = "PlacementBrowser.Tab"))
		FCheckBoxStyle PlacementBrowser_Tab;

	UPROPERTY(Config, EditAnywhere, Category = "Editor Style", meta = (DisplayName = "EditorModesToolbar.ToggleButton"))
		FCheckBoxStyle EditorModesToolbar_ToggleButton;

	UPROPERTY(Config, EditAnywhere, Category = "Editor Style", meta = (DisplayName = "Toolbar.Background"))
		FSlateBrush E_Toolbar_Background;

	UPROPERTY(Config, EditAnywhere, Category = "Editor Style", meta = (DisplayName = "UMGEditor.Palette"))
		FUMGEditorPalette UMGEditor_Palette;

	UPROPERTY(Config, EditAnywhere, Category = "Core Style", meta = (DisplayName = "WindowStyle"))
		FWindowStyle WindowStyle;

	UPROPERTY(Config, EditAnywhere, Category = "Core Style", meta = (DisplayName = "Docking.MajorTab"))
		FDockTabStyle Docking_MajorTab;

	UPROPERTY(Config, EditAnywhere, Category = "Core Style", meta = (DisplayName = "Docking.Tab"))
		FDockTabStyle Docking_Tab;

	UPROPERTY(Config, EditAnywhere, Category = "Core Style", meta = (DisplayName = "TableView.Header"))
		FHeaderRowStyle TableView_Header;

	UPROPERTY(Config, EditAnywhere, Category = "Core Style", meta = (DisplayName = "ToolBar.Background"))
		FSlateBrush ToolBar_Background;	

	UPROPERTY(Config, EditAnywhere, Category = "Core Style", meta = (DisplayName = "Menu.Background"))
		FSlateBrush Menu_Background;

	UPROPERTY(Config, EditAnywhere, Category = "Text Style", meta = (DisplayName = "NormalText"))
		FTextBlockStyle NormalText;

	UPROPERTY(Config, EditAnywhere, Category = "Text Style", meta = (DisplayName = "NormalUnderlinedText"))
		FTextBlockStyle NormalUnderlinedText;
	
	UPROPERTY(Config, EditAnywhere, Category = "Text Style", meta = (DisplayName = "SmallText"))
		FTextBlockStyle SmallText;

	UPROPERTY(Config, EditAnywhere, Category = "Text Style", meta = (DisplayName = "SmallUnderlinedText"))
		FTextBlockStyle SmallUnderlinedText;

	UPROPERTY(Config, EditAnywhere, Category = "Text Style", meta = (DisplayName = "ToolBar.Label"))
		FTextBlockStyle ToolBar_Label;

	UPROPERTY(Config, EditAnywhere, Category = "Text Style", meta = (DisplayName = "Docking.TabFont"))
		FTextBlockStyle Docking_TabFont;

	UPROPERTY(Config, EditAnywhere, Category = "Text Style", meta = (DisplayName = "Menu.Label"))
		FTextBlockStyle Menu_Label;

	UPROPERTY(Config, EditAnywhere, Category = "Text Style", meta = (DisplayName = "Menu.Heading"))
		FTextBlockStyle Menu_Heading;

	UPROPERTY(Config, EditAnywhere, Category = "Text Style", meta = (DisplayName = "PlacementBrowser.Asset.Name"))
		FTextBlockStyle PlacementBrowser_Asset_Name;

	UPROPERTY(Config, EditAnywhere, Category = "Text Style", meta = (DisplayName = "PlacementBrowser.Tab.Text"))
		FTextBlockStyle PlacementBrowser_Tab_Text;

	UPROPERTY(Config, EditAnywhere, Category = "Text Style", meta = (DisplayName = "PropertyWindow"))
		FPropertyWindow PropertyWindow;

	UPROPERTY(Config, EditAnywhere, Category = "Text Style", meta = (DisplayName = "DetailsView.CategoryFontStylet"))
		FSlateFontInfo DetailsView_CategoryFontStyle;

	UPROPERTY(Config, EditAnywhere, Category = "Text Style", meta = (DisplayName = "Hyperlink"))
		FHyperlinkStyle Hyperlink;

	UPROPERTY(Config, EditAnywhere, Category = "Text Style", meta = (DisplayName = "InlineEditableTextBlockStyle"))
		FInlineEditableTextBlockStyle InlineEditableTextBlockStyle;

	UPROPERTY(Config, EditAnywhere, Category = "Text Style", meta = (DisplayName = "SettingsEditor.CatgoryAndSectionFont"))
		FSlateFontInfo SettingsEditor_CatgoryAndSectionFont;

	UPROPERTY(Config, EditAnywhere, Category = "Text Style", meta = (DisplayName = "ContentBrowser"))
		FContentBrowserFont ContentBrowserFont;

	UPROPERTY(Config,EditAnywhere, Category="Advanced Custom Style (Experimental)", meta = (DisplayName = "Editor Style"))
		FUE4ECCustomStyle CustomStyleEditor;

	UPROPERTY(Config, EditAnywhere, Category = "Advanced Custom Style (Experimental)", meta = (DisplayName = "Core Style"))
		FUE4ECCustomStyle CustomStyleCore;

	UEditorCustomizeSetting();

	inline void PostProcessCustomStyle(UScriptStruct* StyleStruct, void* StructPtr);

public:
	void InitEditorStyle();
	void InitCoreStyle();
	void InitTextStyle();
	void InitCustomStyle(class FSlateStyleSet* SlateStyleSet,FUE4ECCustomStyle& CustomStyle);
};

// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "UE4EditorCustomize.h"
#include <IDetailCustomization.h>

/**
 * 
 */
class UE4ECSettingsCustomization :public IDetailCustomization
{
public:
	UE4ECSettingsCustomization();
	~UE4ECSettingsCustomization();
	
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLyoutBuilder) override;
};

/*class UE4ECCustomStyleEditorCustomization :public IDetailCustomization
{
	// Inherited via IDetailCustomization
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
};*/
// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "UE4EditorCustomize.h"
#include <IDetailCustomization.h>

/**
 * 
 */
class SettingsCustomization :public IDetailCustomization
{
public:
	SettingsCustomization();
	~SettingsCustomization();
	
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLyoutBuilder) override;
};

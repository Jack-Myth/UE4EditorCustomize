// Copyright 2018 Jack Myth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ISettingsModule.h"
#include "ModuleManager.h"

class FUE4EditorCustomizeModule : public IModuleInterface
{
	TSharedPtr<ISettingsSection> SettingS;
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	bool OnSettingModified();
	void ApplyEditorStyle(class  UEditorCustomizeSetting* StyleSettings);
	void ApplyCoreStyle(class UEditorCustomizeSetting* StyleSettings);
	void ApplyTextStyle(class UEditorCustomizeSetting* StyleSettings);
};
// Copyright 2018 Jack Myth. All Rights Reserved.

#include "UE4EditorCustomize.h"
#include "EditorStyleSet.h"
#include "Styling/SlateStyle.h"
#include "SlateMaterialBrush.h"
#include "Classes/EditorStyleSettings.h"
#include "Modules/ModuleManager.h"
#include "EditorCustomizeSetting.h"
#include "ISettingsSection.h"
#include "Runtime/Core/Public/Misc/ConfigCacheIni.h"
#include "Misc/MessageDialog.h"
#include "DesktopPlatformModule.h"
#include "Interfaces/IPluginManager.h"
#include <PropertyEditorModule.h>
#include "SettingsCustomization.h"
#include <Engine/AssetManager.h>
#include "Misc/FileHelper.h"
#include "zlib.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#include <AssetToolsModule.h>
#include <ObjectTools.h>
#include <Exporters/Exporter.h>
#include <Engine/Font.h>
#include <Engine/FontFace.h>
#include <Factories/FontFactory.h>
#include "UThemeInfoWindowFactory.h"
#include <Engine/Texture2D.h>
#include <AssetToolsModule.h>
#include <../Launch/Resources/Version.h>
#include <IAssetTools.h>
#include <Editor\UnrealEd\Classes\Factories\FontFileImportFactory.h>

#define LOCTEXT_NAMESPACE "FUE4EditorCustomizeModule"

TArray<UObject*> FUE4EditorCustomizeModule::FindReferencedAsset()
{
	TArray<UObject*> RefSearchResult;
	TArray<UObject*> tmpRefs;
	TArray<UObject*> NeededObject;
	FReferenceFinder ReferenceFinderInstance(RefSearchResult);
	ReferenceFinderInstance.FindReferences(GetMutableDefault<UEditorCustomizeSetting>());
	tmpRefs.Append(RefSearchResult);
	do
	{
		if (tmpRefs[0]->GetPathName().StartsWith("/UE4EditorCustomize/") && tmpRefs[0]->IsAsset() && !NeededObject.Contains(tmpRefs[0]))
		{
			NeededObject.Add(tmpRefs[0]);
			RefSearchResult.Empty();
			ReferenceFinderInstance.FindReferences(tmpRefs[0]);
			tmpRefs.RemoveAt(0);
			tmpRefs.Append(RefSearchResult);
		}
		else
			tmpRefs.RemoveAt(0);
	} while (tmpRefs.Num());
	return NeededObject;
}

FString FUE4EditorCustomizeModule::GetAssetAbsolutePath(FString PackageName)
{
	const FString Extension = FPackageName::GetAssetPackageExtension();
	const FString fFilePath = FPackageName::LongPackageNameToFilename(PackageName, Extension);
	const FString FullFilePath = FPaths::ConvertRelativePathToFull(fFilePath);
	return FullFilePath;
}

//Import UTheme(Version 0)
bool FUE4EditorCustomizeModule::_Internal_ImportUTheme_v0(TArray<uint8>& AllFileData,FText* ErrorMsg)
{
	//UTheme Head(_UTheme)
	//UTheme Version (0)
	//Requested Engine Version(e.g 19) (uint8)
	//Size of Uncompressed Data (int)
	//Data MD5
	//Compressed Data
		//File1 Type (uint8)
		//File1 Data
		//...
		//Config File Type (0)
		//Config File Data
	TArray<uint8> md5byteCheck;
	uint8 FileContentMD5[16];
	int UncompressedFileSize = 0;
	int ContentsDataIndex = 8;
	uint8 RequestEngineVersion=0;
	RequestEngineVersion = AllFileData[ContentsDataIndex];
	ContentsDataIndex++;
	memcpy(&UncompressedFileSize, AllFileData.GetData() + ContentsDataIndex, sizeof(int));
	ContentsDataIndex += 4;
	md5byteCheck.Append(AllFileData.GetData() + ContentsDataIndex, 16);
	ContentsDataIndex += 16;
	FMD5 md5;
	//ContentsDataIndex Current Should be 28
	//8=UTheme Head+File Version,4=UncompressedFileSize,16=md5Byte,28=8+4+16
	md5.Update(AllFileData.GetData() + ContentsDataIndex, AllFileData.Num() - ContentsDataIndex); 
	md5.Final(FileContentMD5);
	if (memcmp(FileContentMD5, md5byteCheck.GetData(), 16))
	{
		if (ErrorMsg)
			*ErrorMsg = LOCTEXT("MD5CheckFailed","MD5 Check Failed, File May Broken.");
		return false;
	}
	if (!_Internal_LoadUThemeInfo(AllFileData, ContentsDataIndex, RequestEngineVersion, 0, AllFileData.Num() / 1024.f / 1024.f))
		return false;
	TArray<uint8> UncompressedDataBuffer;
	//UncompressedDataBuffer.Reserve(UncompressedFileSize);
	UncompressedDataBuffer.SetNum(UncompressedFileSize);
	unsigned long DestL = UncompressedFileSize;
	if (uncompress(UncompressedDataBuffer.GetData(), &DestL, AllFileData.GetData() +ContentsDataIndex, AllFileData.Num() - ContentsDataIndex)!=Z_OK)
		return false;
	ContentsDataIndex = 0;
	if (RequestEngineVersion > FEngineVersion::Current().GetMinor())
	{
		if (ErrorMsg)
			*ErrorMsg = LOCTEXT("RequestHighVerEngine", "This UTheme Request a Higher Engine Version.");
		return false;
	}
	do 
	{
		ContentsDataIndex++;
		switch (UncompressedDataBuffer[ContentsDataIndex - 1])
		{
			case (uint8)AssetType::ConfigIni:
				if (!_Internal_ImportConfigIni(UncompressedDataBuffer, ContentsDataIndex))
					return false;
				break;
			case (uint8)AssetType::Texture2D:
				if (!_Internal_ImportTexture2D(UncompressedDataBuffer, ContentsDataIndex))
					return false;
				break;
			case (uint8)AssetType::Font:
				if (!_Internal_ImportFont(UncompressedDataBuffer, ContentsDataIndex))
					return false;
				break;
			case (uint8)AssetType::UAsset:
				if (!_Internal_ImportUAsset(UncompressedDataBuffer, ContentsDataIndex,ErrorMsg))
					return false;
				break;
		}
	} while (ContentsDataIndex<UncompressedDataBuffer.Num());
	return true;
}

bool FUE4EditorCustomizeModule::_Internal_ExportUAsset(UObject* UAssetObj, TArray<uint8>& UThemeData)
{
	auto& AssetR = UAssetManager::Get().GetAssetRegistry();
	TArray<uint8> FileData;
	FString AssetPathName = AssetR.GetAssetByObjectPath(FName(*UAssetObj->GetPathName())).PackageName.ToString();
	FString FaName = GetAssetAbsolutePath(AssetPathName);
	if (!FFileHelper::LoadFileToArray(FileData, *FaName))
		return false;
	int FileSize = FileData.Num();
	UThemeData.Add(255);
	UThemeData.Append((uint8*)AssetPathName.GetCharArray().GetData(), AssetPathName.Len() * 2 + 2);
	UThemeData.Append((uint8*)&FileSize, sizeof(int));
	UThemeData.Append(FileData);
	return true;
}

bool FUE4EditorCustomizeModule::_Internal_ExportConfigIni(TArray<uint8>& UThemeData)
{
	
	TArray<uint8> FileData;
#if ENGINE_MINOR_VERSION >= 18
	FString ConfigIni = FPaths::ProjectSavedDir() + "/tmpUE4EditorCustomize.ini";
#else
	FString ConfigIni = FPaths::GameSavedDir() + "/tmpUE4EditorCustomize.ini";
#endif
	SettingS->GetSettingsObject()->SaveConfig(CPF_Config, *ConfigIni);
	if (!FFileHelper::LoadFileToArray(FileData, *ConfigIni))
		return false;
	int FileSize = FileData.Num();
	//UThemeData.Append((uint8*)FString("Config.ini").GetCharArray().GetData(), FString("Config.ini").Len() * 2 + 2);
	UThemeData.Add((uint8)AssetType::ConfigIni);
	UThemeData.Append((uint8*)&FileSize, sizeof(int));
	UThemeData.Append(FileData);
	IFileManager::Get().Delete(*ConfigIni, false, false, true);
	return true;
}

bool FUE4EditorCustomizeModule::_Internal_ExportTexture2D(UObject* TextureObj, TArray<uint8>& UThemeData)
{
	//Image Name
	//Image Target Path
	//Image Lens
	//Image Data
	//Image Adjustment (Float)
		//Brightness
		//Brightness Curve
		//Vibrance
		//Saturation
		//RGBCurve
		//HUE
		//Min Alpha
		//Max Alpha
		//Is Chroma Key (uint8 bool)
			//Chroma Key Threshold
			//Chroma Key Color (4*uint8)
				//R
				//G
				//B
				//A
	TArray<uint8> FileData;
	UTexture2D* TextureObject=(UTexture2D*)TextureObj;
	UExporter* NeededExporter = UExporter::FindExporter(TextureObj, TEXT("TGA"));
#if ENGINE_MINOR_VERSION >=18
	FString tmpBMPFileName = FPaths::ProjectSavedDir() + "/tmpUE4EditorCustomize.TGA";
#else
	FString tmpBMPFileName = FPaths::GameSavedDir() + "/tmpUE4EditorCustomize.TGA";
#endif
	auto& AssetR = UAssetManager::Get().GetAssetRegistry();
	FAssetData AssetD = AssetR.GetAssetByObjectPath(FName(*TextureObj->GetPathName()));
	FString AssetName = AssetD.AssetName.ToString();
	FString AssetPathName = AssetD.PackagePath.ToString();
	/*TArray<UExporter*> AllExporter;
	ObjectTools::AssembleListOfExporters(AllExporter);
	for (int i=0;i<AllExporter.Num();i++)
	{
		if (AllExporter[i]->SupportsObject(TextureObj))
		{
			NeededExporter = AllExporter[i];
			break;
		}
	}*/

	if (!NeededExporter)
		return false;
	UExporter::ExportToFile(TextureObj, NeededExporter, *tmpBMPFileName, false);
	if (!FFileHelper::LoadFileToArray(FileData, *tmpBMPFileName))
		return false;
	UThemeData.Add((uint8)AssetType::Texture2D);
	UThemeData.Append((uint8*)AssetName.GetCharArray().GetData(), AssetName.Len()*2 + 2);
	UThemeData.Append((uint8*)AssetPathName.GetCharArray().GetData(), AssetPathName.Len()*2 + 2);
	int FileSize;
	FileSize = FileData.Num();
	UThemeData.Append((uint8*)&FileSize, sizeof(int));
	UThemeData.Append(FileData);
	IFileManager::Get().Delete(*tmpBMPFileName);
	float tmpAdjustmentValue;
	bool tmpAdjustIsChromaKey;
	tmpAdjustmentValue = TextureObject->AdjustBrightness;
	UThemeData.Append((uint8*)&tmpAdjustmentValue, sizeof(float));
	tmpAdjustmentValue = TextureObject->AdjustBrightnessCurve;
	UThemeData.Append((uint8*)&tmpAdjustmentValue, sizeof(float));
	tmpAdjustmentValue = TextureObject->AdjustVibrance;
	UThemeData.Append((uint8*)&tmpAdjustmentValue, sizeof(float));
	tmpAdjustmentValue = TextureObject->AdjustSaturation;
	UThemeData.Append((uint8*)&tmpAdjustmentValue, sizeof(float));
	tmpAdjustmentValue = TextureObject->AdjustRGBCurve;
	UThemeData.Append((uint8*)&tmpAdjustmentValue, sizeof(float));
	tmpAdjustmentValue = TextureObject->AdjustHue;
	UThemeData.Append((uint8*)&tmpAdjustmentValue, sizeof(float));
	tmpAdjustmentValue = TextureObject->AdjustMinAlpha;
	UThemeData.Append((uint8*)&tmpAdjustmentValue, sizeof(float));
	tmpAdjustmentValue = TextureObject->AdjustMaxAlpha;
	UThemeData.Append((uint8*)&tmpAdjustmentValue, sizeof(float));
	tmpAdjustIsChromaKey = TextureObject->bChromaKeyTexture;
	UThemeData.Add((uint8)tmpAdjustIsChromaKey);
	if (tmpAdjustIsChromaKey)
	{
		tmpAdjustmentValue = TextureObject->ChromaKeyThreshold;
		UThemeData.Append((uint8*)&tmpAdjustmentValue, sizeof(float));
		UThemeData.Add(TextureObject->ChromaKeyColor.R);
		UThemeData.Add(TextureObject->ChromaKeyColor.G);
		UThemeData.Add(TextureObject->ChromaKeyColor.B);
		UThemeData.Add(TextureObject->ChromaKeyColor.A);
	}
	return true;
}

bool FUE4EditorCustomizeModule::_Internal_ExportFont(UObject* FontObj, TArray<uint8>& UThemeData, TArray<UObject*>& NeededObject)
{
	//Default Font
	UThemeData.Add((uint8)AssetType::Font);
	for (int i=0; ((UFont*)FontObj)->GetCompositeFont()&&i<((UFont*)FontObj)->GetCompositeFont()->DefaultTypeface.Fonts.Num();i++)
	{
		UThemeData.Add(1);
		const UObject* FontFaceAsset =  ((UFont*)FontObj)->GetCompositeFont()->DefaultTypeface.Fonts[i].Font.GetFontFaceAsset();
		if (FontFaceAsset)
		{
			FString FontName = ((UFont*)FontObj)->GetCompositeFont()->DefaultTypeface.Fonts[i].Name.ToString();
			UThemeData.Append((uint8*)FontName.GetCharArray().GetData(), FontName.Len() *2 + 2);
			if (!_Internal_ExportFontFace(FontFaceAsset, UThemeData))
				return false;
			NeededObject.Remove((UObject*)FontFaceAsset);
		}
	}
	UThemeData.Add(0);		//End Default Font
#if ENGINE_MINOR_VERSION >=19
	//Fallback Font
	for (int i = 0; ((UFont*)FontObj)->GetCompositeFont()&&i <((UFont*)FontObj)->GetCompositeFont()->FallbackTypeface.Typeface.Fonts.Num(); i++)
	{
		UThemeData.Add(1);
		const UObject* FontFaceAsset = ((UFont*)FontObj)->GetCompositeFont()->DefaultTypeface.Fonts[i].Font.GetFontFaceAsset();
		if (FontFaceAsset)
		{
			FString FontName = ((UFont*)FontObj)->GetCompositeFont()->DefaultTypeface.Fonts[i].Name.ToString();
			UThemeData.Append((uint8*)FontName.GetCharArray().GetData(), FontName.Len() *2 + 2);
			if (!_Internal_ExportFontFace(FontFaceAsset, UThemeData))
				return false;
			NeededObject.Remove((UObject*)FontFaceAsset);
		}
	}
#endif
	UThemeData.Add(0);
	FString FontPath;
	FontPath = FontObj->GetPathName();
	UThemeData.Append((uint8*)FontPath.GetCharArray().GetData(), FontPath.Len() *2 + 2);
	return true;
}

bool FUE4EditorCustomizeModule::_Internal_ExportFontFace(const UObject* FontFaceObj, TArray<uint8>& UThemeData)
{
	UFontFace* FontFaceObject = (UFontFace*)FontFaceObj;
	auto& AssetR = UAssetManager::Get().GetAssetRegistry();
	if (!FontFaceObject->FontFaceData->HasData())
		return false;
	//FString AssetPathName = AssetR.GetAssetByObjectPath(FName(*FontFaceObj->GetPathName())).PackageName.ToString();
	FString AssetPathName = FontFaceObj->GetPathName();
	UThemeData.Append((uint8*)AssetPathName.GetCharArray().GetData(), AssetPathName.Len() *2 + 2);
	int FontFaceSize = FontFaceObject->FontFaceData->GetData().Num();
	UThemeData.Append((uint8*)&FontFaceSize, sizeof(int));
	UThemeData.Append(FontFaceObject->FontFaceData->GetData());
	return true;
}

bool FUE4EditorCustomizeModule::_Internal_ImportUAsset(TArray<uint8>& UThemeData, int& Offset, FText* ErrorMsg)
{
	FString AssetPackagePath((TCHAR*)(UThemeData.GetData() + Offset));
	Offset += AssetPackagePath.Len() *2 + 2;
	int AssetSize = 0;
	memcpy(&AssetSize, UThemeData.GetData() + Offset, sizeof(int));
	Offset += sizeof(int);
	TArray<uint8> AssetData;
	AssetData.Append(UThemeData.GetData() + Offset, AssetSize);
	Offset += AssetSize;
	bool SaveResult= FFileHelper::SaveArrayToFile(AssetData, *GetAssetAbsolutePath(AssetPackagePath));
	TArray<FString> PackagePathArray;
	PackagePathArray.Add(FPaths::GetPath(AssetPackagePath));
	UAssetManager::Get().GetAssetRegistry().ScanFilesSynchronous(PackagePathArray,true);
	if (!SaveResult&&ErrorMsg)
		*ErrorMsg = FText::Format(LOCTEXT("ImportAssetError", "Error while importing \"{AssetPath}\".Asset may already exist. Try Delete it Manually."), FText::FromString(AssetPackagePath));
	return SaveResult;
}

bool FUE4EditorCustomizeModule::_Internal_ImportConfigIni(TArray<uint8>& UThemeData, int& Offset)
{
#if ENGINE_MINOR_VERSION >=18
	FString ConfigIni = FPaths::ProjectSavedDir() + "/tmpUE4EditorCustomize"+FString::FromInt(rand())+".ini";
#else
	FString ConfigIni = FPaths::GameSavedDir() + "/tmpUE4EditorCustomize" + FString::FromInt(rand()) + ".ini";
#endif
	int ConfigSize = 0;
	memcpy(&ConfigSize, UThemeData.GetData() + Offset,sizeof(int));
	Offset += sizeof(int);
	TArray<uint8> ConfigFileData;
	ConfigFileData.Append(UThemeData.GetData() + Offset, ConfigSize);
	Offset += ConfigSize;
	if (!FFileHelper::SaveArrayToFile(ConfigFileData, *ConfigIni))
		return false;
	bool Succeed;
	Succeed  = ImportSettingFromIni(ConfigIni);
	IFileManager::Get().Delete(*ConfigIni, false, false, true);
	return Succeed;
}

bool FUE4EditorCustomizeModule::_Internal_ImportTexture2D(TArray<uint8>& UThemeData, int& Offset)
{
	//Image Name
	//Image Target Path
	//Image Lens
	//Image Data
	//Image Adjustment (Float)
		//Brightness
		//Brightness Curve
		//Vibrance
		//Saturation
		//RGBCurve
		//HUE
		//Min Alpha
		//Max Alpha
		//Is Chroma Key (uint8 bool)
			//Chroma Key Threshold
			//Chroma Key Color (4*uint8)
				//R
				//G
				//B
				//A
	FString TextureName((TCHAR*)(UThemeData.GetData()+Offset));
	Offset += TextureName.Len() *2 + 2;
	FString TexturePath((TCHAR*)(UThemeData.GetData() + Offset));
	Offset += TexturePath.Len() *2 + 2;
#if ENGINE_MINOR_VERSION >=18
	FString TextureTGAFileName = FPaths::ProjectSavedDir() + TextureName+".TGA";
#else
	FString TextureTGAFileName = FPaths::GameSavedDir() + TextureName + ".TGA";
#endif
	int TextureSize = 0;
	memcpy(&TextureSize, UThemeData.GetData() + Offset, sizeof(int));
	Offset += sizeof(int);
	TArray<uint8> TextureData;
	TextureData.Append(UThemeData.GetData() + Offset, TextureSize);
	Offset += TextureSize;
	if (!FFileHelper::SaveArrayToFile(TextureData, *TextureTGAFileName))
		return false;
	TArray<FString> TextureBMPFileNameCollection;
	TextureBMPFileNameCollection.Add(TextureTGAFileName);
	TArray<UObject*> AssetTextureObj;
	AssetTextureObj=FAssetToolsModule::GetModule().Get().ImportAssets(TextureBMPFileNameCollection, TexturePath);
	IFileManager::Get().Delete(*TextureTGAFileName);
	if (!AssetTextureObj.Num())
	{
		Offset += 8 * sizeof(float);
		if (UThemeData[Offset])
			Offset += sizeof(float) + 4;
		Offset++;
		return true;
	}
	float tmpAdjustFloatValue;
	UTexture2D* TextureObj = (UTexture2D*)AssetTextureObj[0];
	memcpy(&tmpAdjustFloatValue, UThemeData.GetData() + Offset, sizeof(float));
	TextureObj->AdjustBrightness = tmpAdjustFloatValue;
	Offset += sizeof(float);
	memcpy(&tmpAdjustFloatValue, UThemeData.GetData() + Offset, sizeof(float));
	TextureObj->AdjustBrightnessCurve = tmpAdjustFloatValue;
	Offset += sizeof(float);
	memcpy(&tmpAdjustFloatValue, UThemeData.GetData() + Offset, sizeof(float));
	TextureObj->AdjustVibrance = tmpAdjustFloatValue;
	Offset += sizeof(float);
	memcpy(&tmpAdjustFloatValue, UThemeData.GetData() + Offset, sizeof(float));
	TextureObj->AdjustSaturation = tmpAdjustFloatValue;
	Offset += sizeof(float);
	memcpy(&tmpAdjustFloatValue, UThemeData.GetData() + Offset, sizeof(float));
	TextureObj->AdjustRGBCurve = tmpAdjustFloatValue;
	Offset += sizeof(float);
	memcpy(&tmpAdjustFloatValue, UThemeData.GetData() + Offset, sizeof(float));
	TextureObj->AdjustHue = tmpAdjustFloatValue;
	Offset += sizeof(float);
	memcpy(&tmpAdjustFloatValue, UThemeData.GetData() + Offset, sizeof(float));
	TextureObj->AdjustMinAlpha= tmpAdjustFloatValue;
	Offset += sizeof(float);
	memcpy(&tmpAdjustFloatValue, UThemeData.GetData() + Offset, sizeof(float));
	TextureObj->AdjustMaxAlpha = tmpAdjustFloatValue;
	Offset += sizeof(float);
	uint8 IsChromaKeyTexture;
	IsChromaKeyTexture = UThemeData[Offset];
	TextureObj->bChromaKeyTexture = (IsChromaKeyTexture==0?false:true);
	Offset ++;
	if (IsChromaKeyTexture)
	{
		memcpy(&tmpAdjustFloatValue, UThemeData.GetData() + Offset, sizeof(float));
		TextureObj->ChromaKeyThreshold = tmpAdjustFloatValue;
		Offset += sizeof(float);
		TextureObj->ChromaKeyColor.R = UThemeData[Offset];
		TextureObj->ChromaKeyColor.G = UThemeData[Offset + 1];
		TextureObj->ChromaKeyColor.B = UThemeData[Offset + 2];
		TextureObj->ChromaKeyColor.A = UThemeData[Offset + 3];
		Offset += 4;
	}
	TextureObj->UpdateResource();
	return true;
}

bool FUE4EditorCustomizeModule::_Internal_ImportFont(TArray<uint8>& UThemeData, int& Offset)
{
	TMap<FString,UFontFace*> FontPair;
	UFont* tmpFont = NewObject<UFont>();
	tmpFont->FontCacheType = EFontCacheType::Runtime;
	//Default Font
	while (UThemeData[Offset])
	{
		Offset++;
		FString FontFacePath;
		FontFacePath = FString((TCHAR*)(UThemeData.GetData() + Offset));
		Offset += FontFacePath.Len() * 2 + 2;
		UFontFace* FontFaceInstance=nullptr;
		FontFaceInstance = _Internal_ImportFontFace(UThemeData, Offset);
		if (!FontFaceInstance)
			return false;
		FontPair.Add(FontFacePath, FontFaceInstance);
	}
	Offset++;
	for (auto it=FontPair.CreateConstIterator();it;++it)
	{
		FTypefaceEntry tmpTypeFaceEntry(*(it->Key));
		tmpTypeFaceEntry.Font = FFontData(it->Value);
		tmpFont->CompositeFont.DefaultTypeface.Fonts.Add(tmpTypeFaceEntry);
	}
	FontPair.Empty();
	while (UThemeData[Offset])
	{
		Offset++;
		FString FontFacePath;
		FontFacePath = FString((TCHAR*)(UThemeData.GetData() + Offset));
		Offset += FontFacePath.Len() * 2 + 2;
		UFontFace* FontFaceInstance = nullptr;
		FontFaceInstance = _Internal_ImportFontFace(UThemeData, Offset);
		if (!FontFaceInstance)
			return false;
		FontPair.Add(FontFacePath, FontFaceInstance);
	}
	Offset++;
#if ENGINE_MINOR_VERSION >=19
	for (auto it = FontPair.CreateConstIterator(); it; ++it)
	{
		FTypefaceEntry tmpTypeFaceEntry(*(it->Key));
		tmpTypeFaceEntry.Font = FFontData(it->Value);
		tmpFont->CompositeFont.FallbackTypeface.Typeface.Fonts.Add(tmpTypeFaceEntry);
	}
#endif
	FString AssetPathName;
	AssetPathName = FString((TCHAR*)(UThemeData.GetData() + Offset));
	Offset += AssetPathName.Len() *2 + 2;
	FString AssetPackageName = FPackageName::ObjectPathToPackageName(AssetPathName);
	FText errorMsg;
#if ENGINE_MINOR_VERSION >=18
	if (!FFileHelper::IsFilenameValidForSaving(AssetPackageName, errorMsg))
		return false;
#endif
	FString AssetName = FPackageName::GetLongPackageAssetName(AssetPackageName);
	//the Asset package may already exist
	UPackage* AssetPackage = FindPackage(nullptr, *AssetPackageName);
	if (!AssetPackage)
		AssetPackage = CreatePackage(*AssetPackageName);
	UFont* NewFontAsset = Cast<UFont>(StaticDuplicateObject(tmpFont, AssetPackage,*AssetName));
	if (!NewFontAsset)
		return false;
	NewFontAsset->SetFlags(RF_Public | RF_Standalone);
	NewFontAsset->MarkPackageDirty();
	FAssetRegistryModule::AssetCreated(NewFontAsset);
	return true;
}

UFontFace* FUE4EditorCustomizeModule::_Internal_ImportFontFace(TArray<uint8>& UThemeData, int& Offset)
{
	FString AssetPathName;
	AssetPathName = FString((TCHAR*)(UThemeData.GetData() + Offset));
	Offset += AssetPathName.Len() *2 + 2;
	int FontDataSize = 0;
	memcpy(&FontDataSize, UThemeData.GetData() + Offset, sizeof(int));
	Offset += sizeof(int);
	int FontDataBeginIndex = Offset;
	Offset += FontDataSize;
	FString AssetPackageName = FPackageName::ObjectPathToPackageName(AssetPathName);
	FText errorMsg;
#if ENGINE_MINOR_VERSION >=18
	if (!FFileHelper::IsFilenameValidForSaving(AssetPackageName, errorMsg))
		return false;
#endif
	FString AssetName = FPackageName::GetLongPackageAssetName(AssetPackageName);
	UPackage* AssetPackage = CreatePackage(*AssetPackageName);
	auto* FontFaceFactory = GetMutableDefault<UFontFileImportFactory>();
	const uint8* FontDataBegin = UThemeData.GetData() + FontDataBeginIndex;
	bool PrevAutoState = GIsAutomationTesting;
	GIsAutomationTesting = true;
	UObject* NewFontFaceAsset = FontFaceFactory->FactoryCreateBinary(
		UFontFace::StaticClass(), AssetPackage, *AssetName, RF_Public | RF_Standalone,
		nullptr, *UFontFace::StaticClass()->GetName(), FontDataBegin, FontDataBegin + FontDataSize, nullptr);
	GIsAutomationTesting = PrevAutoState;
	NewFontFaceAsset->MarkPackageDirty();
	FAssetRegistryModule::AssetCreated(NewFontFaceAsset);
	return Cast<UFontFace>(NewFontFaceAsset);
}

bool FUE4EditorCustomizeModule::_Internal_LoadUThemeInfo(TArray<uint8>& UThemeData, int& Offset, int RequestVersion, int UThemeVersion, float Size)
{
	FUThemeInfo_v0 UThemeInfo;
	UThemeInfoWindowFactory UTIWF(false);
	UThemeInfo.Name = FString((TCHAR*)(UThemeData.GetData()+Offset));
	Offset += UThemeInfo.Name.Len() * 2 + 2;
	UThemeInfo.Author = FString((TCHAR*)(UThemeData.GetData() + Offset));
	Offset += UThemeInfo.Author.Len() * 2 + 2;
	UThemeInfo.Intro = FString((TCHAR*)(UThemeData.GetData() + Offset));
	Offset += UThemeInfo.Intro.Len() * 2 + 2;
	UThemeInfo.Size = Size;
	UThemeInfo.RequestEngineVersion = RequestVersion;
	UThemeInfo.Version = UThemeVersion;
	int IconImageSize;
	memcpy(&IconImageSize, UThemeData.GetData() + Offset, sizeof(int));
	Offset += sizeof(int);
	if (IconImageSize>0)
	{
		uint8 ExtensionType;
		ExtensionType = UThemeData[Offset];
		Offset++;
		TArray<uint8> ImageData;
		ImageData.Append(UThemeData.GetData() + Offset, IconImageSize);
		Offset += IconImageSize;
		UTIWF.SetIconImage(UThemeInfoWindowFactory::GetLocalTexture(ImageData, Uint8ToExtension(ExtensionType)));
	}
	int PreviewImageCount;
	memcpy(&PreviewImageCount, UThemeData.GetData() + Offset, sizeof(int));
	Offset += sizeof(int);
	for (int i = 0; i < PreviewImageCount; i++)
	{
		int ImageLens;
		memcpy(&ImageLens, UThemeData.GetData() + Offset, sizeof(int));
		Offset += sizeof(int);
		uint8 ExtensionType;
		ExtensionType = UThemeData[Offset];
		Offset++;
		TArray<uint8> ImageData;
		ImageData.Append(UThemeData.GetData() + Offset, ImageLens);
		Offset += ImageLens;
		UTIWF.AddPreviewPicture(UThemeInfoWindowFactory::GetLocalTexture(ImageData, Uint8ToExtension(ExtensionType)),false);
	}
	UTIWF.ApplyUThemeInfo(UThemeInfo);
	return UTIWF.ShowModalDialog();
}

bool FUE4EditorCustomizeModule::_Internal_AskUThemeInfo(FUThemeInfo_v0* OutInfo)
{
	if (!OutInfo)
		return false;
	UThemeInfoWindowFactory InfoInputWindow(true);
	bool IsConfirmed=InfoInputWindow.ShowModalDialog();
	if (IsConfirmed)
		*OutInfo = InfoInputWindow.GetInfomation();
	return IsConfirmed;
}

uint8 FUE4EditorCustomizeModule::ExtensionToUint8(FString FileExtension)
{
	if (FileExtension.Equals("JPG", ESearchCase::IgnoreCase) || FileExtension.Equals("JPEG", ESearchCase::IgnoreCase))
		return 0;
	else if (FileExtension.Equals("PNG", ESearchCase::IgnoreCase))
		return 1;
	else
		return 2;
}

FString FUE4EditorCustomizeModule::Uint8ToExtension(uint8 Extension)
{
	switch (Extension)
	{
		case 0:
			return "JPEG";
		case 1:
			return "PNG";
		default:
			return "BMP";
	}
}

void FUE4EditorCustomizeModule::CacheOriginalBrushes()
{
	FSlateStyleSet* EditorStyles = (FSlateStyleSet*)&FEditorStyle::Get();
	const FName BrushesName[]=
	{
		"Graph.Panel.SolidBackground", 
		"ToolPanel.GroupBorder", 
		"ToolPanel.DarkGroupBorder", 
		"SCSEditor.TreePanel", 
		"SettingsEditor.CheckoutWarningBorder", 
		"DetailsView.CategoryTop_Hovered", 
		"DetailsView.CategoryTop", 
		"DetailsView.CategoryMiddle_Hovered",
		"DetailsView.CategoryMiddle",
		"DetailsView.CategoryMiddle_Highlighted",
		"DetailsView.CollapsedCategory_Hovered",
		"DetailsView.CollapsedCategory", 
		"DetailsView.CategoryBottom",
		"DetailsView.AdvancedDropdownBorder",
		"Toolbar.Background",
		"Docking.Tab.ContentAreaBrush",
		"ContentBrowser.TopBar.GroupBorder",
		"MessageLog.ListBorder"
	};
	for (const FName& CurName : BrushesName)
	{
		const FSlateBrush* tmpCachedBrush = EditorStyles->GetBrush(CurName);
		CachedOriginalBrushes.FindOrAdd(CurName) = tmpCachedBrush;
	}
	ReCacheCustomBrushes();
}

void FUE4EditorCustomizeModule::ReCacheCustomBrushes()
{
	UEditorCustomizeSetting* StyleSettings = GetMutableDefault<UEditorCustomizeSetting>();
	FSlateStyleSet* EditorStyles = (FSlateStyleSet*)&FEditorStyle::Get();
	FSlateStyleSet* CoreStyles = (FSlateStyleSet*)&FCoreStyle::Get();
	auto tmpFunc = [](
		TMap<FName, const FSlateBrush*>& CachedCustomBrushes,
		FSlateStyleSet* SlateStyleSet,
		TMap<FName, FSlateBrush>& SlateBrushes)
	{
		TArray<FName> tmpBrush;
		TArray<FName> tmpNeedCachedBrush;
		for (const auto& CurBrushPair : SlateBrushes)
		{
			if (!CachedCustomBrushes.Find(CurBrushPair.Key))
				tmpNeedCachedBrush.Add(CurBrushPair.Key);
			else
				tmpBrush.Add(CurBrushPair.Key);
		}
		//Release the brush that's no longer need to be cached.
		for (const FName& NeedReleasedBrush : tmpBrush)
		{
			const FSlateBrush* copiedValue;
			CachedCustomBrushes.RemoveAndCopyValue(NeedReleasedBrush, copiedValue);
			SlateStyleSet->Set(NeedReleasedBrush, (FSlateBrush*)copiedValue);
		}
		//ReCache the brushes.
		for (const FName& NeedCachedKey : tmpNeedCachedBrush)
		{
			const FSlateBrush* originalBrush = SlateStyleSet->GetBrush(NeedCachedKey);
			if (!originalBrush)
				continue; //Don't cache if it's a wrong key
			CachedCustomBrushes.Add(NeedCachedKey) = originalBrush;
			SlateStyleSet->Set(NeedCachedKey, SlateBrushes.Find(NeedCachedKey));
		}
	};
	tmpFunc(CachedCustomBrushesEditor, EditorStyles, StyleSettings->CustomStyleEditor.SlateBrush);
	tmpFunc(CachedCustomBrushesCore, CoreStyles, StyleSettings->CustomStyleCore.SlateBrush);
}

void FUE4EditorCustomizeModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	//Cache Original Brushes first.
	//Prepare for ShutdownModule()
	CacheOriginalBrushes();

	UEditorCustomizeSetting* StyleSettings = GetMutableDefault<UEditorCustomizeSetting>();
	GetMutableDefault<UEditorStyleSettings>()->bUseGrid = StyleSettings->EditorUseGrid;
	ApplyCoreStyle(StyleSettings);
	ApplyEditorStyle(StyleSettings);
	ApplyTextStyle(StyleSettings);
	ApplyCustomStyle(StyleSettings);
	//ReCacheCustomBrushes();
	FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor").RegisterCustomClassLayout(UEditorCustomizeSetting::StaticClass()->GetFName(),
																										 FOnGetDetailCustomizationInstance::CreateLambda([]() {return MakeShareable(new UE4ECSettingsCustomization); }));
	SettingS = FModuleManager::LoadModuleChecked<ISettingsModule>("Settings").RegisterSettings("Project", "Plugins",
																					TEXT("UE4EditorCustomize"), FText::FromString("UE4 EditorCustomize"),
																					FText::FromString("Setting For UE4EditorCustomize"), GetMutableDefault<UEditorCustomizeSetting>());
	SettingS->OnModified().BindRaw(this, &FUE4EditorCustomizeModule::OnSettingModified);
	SettingS->OnSaveDefaults().BindLambda([=]()->bool
		{
			GetMutableDefault<UEditorCustomizeSetting>()->UpdateGlobalUserConfigFile();
			return true;
		});
	SettingS->OnResetDefaults().BindLambda([=]()->bool
		{
			TArray<FString> ConfigNeedToReset;
			FString ConfigSectionName = GetMutableDefault<UEditorCustomizeSetting>()->GetClass()->GetPathName();
			ConfigNeedToReset.Add(GetMutableDefault<UEditorCustomizeSetting>()->GetClass()->GetConfigName());
			ConfigNeedToReset.Add(GetMutableDefault<UEditorCustomizeSetting>()->GetGlobalUserConfigFilename());
			for (FString ConfigFileName : ConfigNeedToReset)
			{
				GConfig->EmptySection(*ConfigSectionName, ConfigFileName);
				GConfig->Flush(false);
			}
			FConfigCacheIni::LoadGlobalIniFile(ConfigNeedToReset[0], *FPaths::GetBaseFilename(ConfigNeedToReset[0]), nullptr, true);
			GetMutableDefault<UEditorCustomizeSetting>()->ReloadConfig(nullptr, nullptr, UE4::LCPF_PropagateToInstances | UE4::LCPF_PropagateToChildDefaultObjects);
			FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ConfigReset_RestartEditor","Config has been reset.Restart Editor to take effect."));
			return true;
		});
}

void FUE4EditorCustomizeModule::ShutdownModule()
{
	//Restore Cached Brush
	//Editor will delete all editor brushes when exit.
	//So if the brushes didn't restore, It will cause Editor Crash.
	RestoreCachedBrush();
	FModuleManager::LoadModuleChecked<ISettingsModule>("Settings").UnregisterSettings("Project", "Plugins", "UE4EditorCustomize");
}

bool FUE4EditorCustomizeModule::OnSettingModified()
{
	UEditorCustomizeSetting* StyleSettings = GetMutableDefault<UEditorCustomizeSetting>();
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

	//Check if User add a new style
	StyleSettings->InitCustomStyle((FSlateStyleSet*)&FEditorStyle::Get(), StyleSettings->CustomStyleEditor);
	StyleSettings->InitCustomStyle((FSlateStyleSet*)&FCoreStyle::Get(), StyleSettings->CustomStyleCore);
	ApplyCustomStyle(StyleSettings);
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
	EditorStyles->Set("Docking.Tab.ContentAreaBrush", &StyleSettings->Docking_Tab_ContentAreaBrush);
	EditorStyles->Set("ContentBrowser.TopBar.GroupBorder", &StyleSettings->ContentBrowser_TopBar_GroupBorder);
	EditorStyles->Set("MessageLog.ListBorder", &StyleSettings->MessageLog_ListBorder);
	(FCheckBoxStyle&)FEditorStyle::GetWidgetStyle<FCheckBoxStyle>("PlacementBrowser.Tab") = StyleSettings->PlacementBrowser_Tab;
	(FCheckBoxStyle&)FEditorStyle::GetWidgetStyle<FCheckBoxStyle>("EditorModesToolbar.ToggleButton") = StyleSettings->EditorModesToolbar_ToggleButton;
	(FTableRowStyle&)FEditorStyle::GetWidgetStyle<FTableRowStyle>("TableView.DarkRow") = StyleSettings->TableView_DarkRow;
	(FTableRowStyle&)FEditorStyle::GetWidgetStyle<FTableRowStyle>("UMGEditor.PaletteHeader") = StyleSettings->UMGEditor_Palette.UMGEditor_PaletteHeader;
	(FTableRowStyle&)FEditorStyle::GetWidgetStyle<FTableRowStyle>("UMGEditor.PaletteItem") = StyleSettings->UMGEditor_Palette.UMGEditor_PaletteItem;
	(FEditableTextBoxStyle&)FEditorStyle::GetWidgetStyle<FEditableTextBoxStyle>("Log.TextBox") = StyleSettings->Log_TextBox;
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
	(FTextBlockStyle&)FEditorStyle::GetWidgetStyle<FTextBlockStyle>("Log.Normal") = StyleSettings->Log_Normal;
}

void FUE4EditorCustomizeModule::ApplyCustomStyle(class UEditorCustomizeSetting* StyleSettings)
{
	ReCacheCustomBrushes();
	//Apply Font
	FSlateStyleSet* EditorStyles = (FSlateStyleSet*)&FEditorStyle::Get();
	FSlateStyleSet* CoreStyles = (FSlateStyleSet*)&FCoreStyle::Get();
	auto tmpFunc = [&StyleSettings](FSlateStyleSet* SlateStyleSet, FUE4ECCustomStyle& CustomStyle)
	{
		//Apply Font First
		for (auto& FontPair : CustomStyle.SlateFontInfo)
			SlateStyleSet->Set(FontPair.Key, FontPair.Value);
#define APPLY_STYLE(STYLE_NAME) for (auto& curStylePair:CustomStyle.STYLE_NAME) \
		if (SlateStyleSet->HasWidgetStyle<F##STYLE_NAME>(curStylePair.Key))\
			(F##STYLE_NAME&)SlateStyleSet->GetWidgetStyle<F##STYLE_NAME>(curStylePair.Key) = curStylePair.Value
		APPLY_STYLE(ButtonStyle);
		APPLY_STYLE(ComboBoxStyle);
		APPLY_STYLE(ComboButtonStyle);
		APPLY_STYLE(DockTabStyle);
		APPLY_STYLE(EditableTextBoxStyle);
		APPLY_STYLE(EditableTextStyle);
		APPLY_STYLE(ExpandableAreaStyle);
		APPLY_STYLE(HeaderRowStyle);
		APPLY_STYLE(InlineEditableTextBlockStyle);
		APPLY_STYLE(InlineTextImageStyle);
		APPLY_STYLE(ProgressBarStyle);
		APPLY_STYLE(ScrollBarStyle);
		APPLY_STYLE(ScrollBorderStyle);
		APPLY_STYLE(ScrollBoxStyle);
		APPLY_STYLE(SearchBoxStyle);
		APPLY_STYLE(SliderStyle);
		APPLY_STYLE(SpinBoxStyle);
		APPLY_STYLE(SplitterStyle);
		APPLY_STYLE(TableColumnHeaderStyle);
		APPLY_STYLE(TableRowStyle);
		APPLY_STYLE(TextBlockStyle);
		APPLY_STYLE(VolumeControlStyle);
		APPLY_STYLE(WindowStyle);
#undef APPLY_STYLE
	};
	tmpFunc(EditorStyles, StyleSettings->CustomStyleEditor);
	tmpFunc(EditorStyles, StyleSettings->CustomStyleCore);
}

bool FUE4EditorCustomizeModule::ImportSettingFromIni(FString FilePath)
{
	return (SettingS->Import(FilePath) && SettingS->Save());
}

bool FUE4EditorCustomizeModule::ImportUTheme(FString FilePath, FText* ErrorMsg)
{
	TArray<uint8> FileData;
	if (ErrorMsg)
		*ErrorMsg = LOCTEXT("ImportFailed", "Import Failed.");
	if (!FFileHelper::LoadFileToArray(FileData, *FilePath))
	{
		if (ErrorMsg)
			*ErrorMsg = LOCTEXT("FailedToLoadFile", "Failed To Load File.");
		return false;
	}
	constexpr char* UThemeHead = "_UTheme";
	if (memcmp(FileData.GetData(), UThemeHead, 7))
	{
		if (ErrorMsg)
			*ErrorMsg = LOCTEXT("InvalidUThemeFile", "Invalid UTheme File.");
		return false;
	}
	switch (FileData[7])
	{
		case 0:
			if (!_Internal_ImportUTheme_v0(FileData,ErrorMsg))
				return false;
			break;
		default:
			if (ErrorMsg)
				*ErrorMsg = LOCTEXT("UnsupportedUThemeVersion", " Unsupported UTheme Version.");
			return false;
			break;
	}
	if (ErrorMsg)
		*ErrorMsg = FText::FromString("");
	return true;
}

void FUE4EditorCustomizeModule::ShowDialogForImport()
{
	TArray<FString> OutFiles;
	if (FDesktopPlatformModule::Get()->OpenFileDialog(NULL, "Import Theme",
													  IPluginManager::Get().FindPlugin(TEXT("UE4EditorCustomize"))->GetBaseDir() + FString("/Themes")
													  , TEXT(""), TEXT("All(*.ini,*.UTheme)|*.ini;*.UTheme|Theme File (*.UTheme)|*.UTheme|Config File(*.ini)|*.ini"), EFileDialogFlags::None, OutFiles))
	{ 
		if (FPaths::GetExtension(OutFiles[0]).Equals("UTheme",ESearchCase::IgnoreCase))
		{
			FText ErrorMsg;
			if (!ImportUTheme(OutFiles[0], &ErrorMsg))
			{
				FText tmpText = LOCTEXT("ImportFailed", "Import Failed.");
				FMessageDialog::Open(EAppMsgType::Ok, ErrorMsg, &tmpText);
			}
			else
				FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ImportSucceed", "Import Succeed."));
		}
		else if (FPaths::GetExtension(OutFiles[0]).Equals("INI", ESearchCase::IgnoreCase))
		{
			if (!ImportSettingFromIni(OutFiles[0]))
				FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ImportFailed", "Import Failed."));
			else
				FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ImportSucceed", "Import Succeed.")); 
		}
	}
}

void FUE4EditorCustomizeModule::ImportBuiltInTheme()
{
	if (ImportUTheme(IPluginManager::Get().FindPlugin(TEXT("UE4EditorCustomize"))->GetBaseDir() + FString("/Resources/HatsuneMiku.UTheme")))
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ImportSucceed","Import Succeed."));
	}
	else
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ImportFailed", "Import Failed."));
	}
}

bool FUE4EditorCustomizeModule::PackageTheme(FString FilePath, FUThemeInfo_v0 UThemeInfo)
{
	//UTheme Head(_UTheme)
	//UTheme Version (0)
	//Requested Engine Version(e.g 19) (uint8)
	//Size of Uncompressed Data (int)
	//Remain Data MD5
	//UThemeInfo Data (except Request Engine Version)
	//Name,Author,Intro
	//Image Lens
	//Icon Image Type (uint8)
	//Image Data
	//Preview Image Count
	//Image Lens
	//Image Type
	//Image Data
	//...
	//Compressed Data
		//File1 Type (uint8)
		//File1 ExportData
		//...
		//Config File Type (0)
		//Config File ExportData
	TArray<FAssetData> PluginContentsAssetData;
	TArray<UObject*> NeededObject = FindReferencedAsset();
	TArray<uint8> UThemeFile;
	TArray<uint8> UThemeInfoData;
	TArray<uint8> UThemeData;
	constexpr char* UThemeHead = "_UTheme";
	UThemeFile.Append((uint8*)UThemeHead, 7);  //UTheme Head(_UTheme)
	UThemeFile.Add(0);		//UTheme Version.
	bool RequestEngineVersion = false;
	uint8 tmpZero[2] = { 0,0 };
	UThemeInfoData.Append(UThemeInfo.Name.Len()>0?(uint8*)UThemeInfo.Name.GetCharArray().GetData(): tmpZero, UThemeInfo.Name.Len() * 2 + 2);
	UThemeInfoData.Append(UThemeInfo.Author.Len()>0 ? (uint8*)UThemeInfo.Author.GetCharArray().GetData() : tmpZero, UThemeInfo.Author.Len() * 2 + 2);
	UThemeInfoData.Append(UThemeInfo.Intro.Len()>0 ? (uint8*)UThemeInfo.Intro.GetCharArray().GetData() : tmpZero, UThemeInfo.Intro.Len() * 2 + 2);
	if (UThemeInfo.IconImagePath=="")
	{
		//Prevent err C4456
		int _tmpZero = 0;
		UThemeInfoData.Append((uint8*)&_tmpZero, sizeof(int));
	}
	else
	{
		int ImageSize = 0;
		TArray<uint8> ImageData;
		if (!FFileHelper::LoadFileToArray(ImageData, *(UThemeInfo.IconImagePath)))
			return false;
		ImageSize = ImageData.Num();
		UThemeInfoData.Append((uint8*)&ImageSize, sizeof(int));
		UThemeInfoData.Add(ExtensionToUint8(FPaths::GetExtension(UThemeInfo.IconImagePath)));
		UThemeInfoData.Append(ImageData);
	}
	{
		int PreviewCount = UThemeInfo.tmpImagePath.Num();
		UThemeInfoData.Append((uint8*)&PreviewCount, sizeof(int));
		for (int i = 0; i < UThemeInfo.tmpImagePath.Num(); i++)
		{
			TArray<uint8> ImageData;
			int ImageSize = 0;
			if (!FFileHelper::LoadFileToArray(ImageData, *(UThemeInfo.tmpImagePath[i])))
				return false;
			ImageSize = ImageData.Num();
			UThemeInfoData.Append((uint8*)&ImageSize, sizeof(int));
			UThemeInfoData.Add(ExtensionToUint8(FPaths::GetExtension(UThemeInfo.tmpImagePath[i])));
			UThemeInfoData.Append(ImageData);
		}
	}
	for (int i=0;i<NeededObject.Num();i++)
	{
		bool IsSucceed = false;
		if (NeededObject[i]->GetClass()->GetName() == "Texture2D")
			IsSucceed= _Internal_ExportTexture2D(NeededObject[i], UThemeData);
		else if (NeededObject[i]->GetClass()->GetName() == "Font")
			IsSucceed= _Internal_ExportFont(NeededObject[i], UThemeData, NeededObject);
		else if (NeededObject[i]->GetClass()->GetName() == "FontFace")
			continue;		//FontFace will be Exported By _Internal_ExportFont
		else
		{
			RequestEngineVersion = true;
			IsSucceed = _Internal_ExportUAsset(NeededObject[i], UThemeData);
		}
		if (!IsSucceed)
			return false;
	}
	_Internal_ExportConfigIni(UThemeData);
	if (RequestEngineVersion)
		UThemeFile.Add((uint8)FEngineVersion::Current().GetMinor());
	else
		UThemeFile.Add(0);
	int UThemeDataSize = UThemeData.Num();
	UThemeFile.Append((uint8*)&UThemeDataSize, sizeof(int)); //Size of Uncompressed Data (int)
	uint8* DataBuffer = new uint8[UThemeData.Num()];
	unsigned long DestLen = (unsigned long)UThemeData.Num();
	if (compress2(DataBuffer, &DestLen, UThemeData.GetData(), UThemeData.Num(), Z_BEST_COMPRESSION) != Z_OK)
	{
		delete DataBuffer;
		return false;
	}
	UThemeData.Empty();
	UThemeData.Append(UThemeInfoData);
	UThemeData.Append(DataBuffer, (int32)DestLen);
	delete DataBuffer;
	FMD5 md5;
	md5.Update(UThemeData.GetData(), UThemeData.Num());
	uint8 md5Bytes[16];
	md5.Final(md5Bytes);
	UThemeFile.Append(md5Bytes, 16);
	UThemeFile.Append(UThemeData);
	FFileHelper::SaveArrayToFile(UThemeFile, *FilePath);
	//const FString PackageName = PluginContentsAssetData[0].PackageName.ToString();
	
	//FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(FullFilePath));
	return true;
}

void FUE4EditorCustomizeModule::ShowDialogForPackageTheme()
{
	FUThemeInfo_v0 UThemeInfo;
	if (!_Internal_AskUThemeInfo(&UThemeInfo))
		return;
	TArray<FString> OutFiles;
	if (FDesktopPlatformModule::Get()->SaveFileDialog(NULL, "Package Theme", 
													  FPaths::GetProjectFilePath(),TEXT(""), 
													  TEXT("Theme File (*.UTheme)|*.UTheme"), EFileDialogFlags::None, OutFiles))
	{
		if (PackageTheme(OutFiles[0], UThemeInfo))
		{
			FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("PackageSucceed","Package Succeed."));
		}
		else
		{
			FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("PackageFailed", "Package Failed."));
		}
	}
}

void FUE4EditorCustomizeModule::ResetEditorStyle()
{
	FString ConfigName = GetMutableDefault<UEditorCustomizeSetting>()->GetClass()->GetConfigName();
	FString Sec = GetMutableDefault<UEditorCustomizeSetting>()->GetClass()->GetPathName();
	GConfig->RemoveKey(*Sec, TEXT("EditorUseGrid"), ConfigName);
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
	GConfig->RemoveKey(*Sec, TEXT("Docking_Tab_ContentAreaBrush"), ConfigName);
	GConfig->RemoveKey(*Sec, TEXT("ContentBrowser_TopBar_GroupBorder"), ConfigName);
	GConfig->RemoveKey(*Sec, TEXT("MessageLog_ListBorder"), ConfigName);
	GConfig->RemoveKey(*Sec, TEXT("Log_TextBox"), ConfigName);
	GConfig->Flush(false);
}

void FUE4EditorCustomizeModule::ResetCoreStyle()
{
	FString ConfigName = GetMutableDefault<UEditorCustomizeSetting>()->GetClass()->GetConfigName();
	FString Sec = GetMutableDefault<UEditorCustomizeSetting>()->GetClass()->GetPathName();
	GConfig->RemoveKey(*Sec, TEXT("WindowStyle"), ConfigName);
	GConfig->RemoveKey(*Sec, TEXT("Docking_MajorTab"), ConfigName);
	GConfig->RemoveKey(*Sec, TEXT("Docking_Tab"), ConfigName);
	GConfig->RemoveKey(*Sec, TEXT("ToolBar_Background"), ConfigName);
	GConfig->RemoveKey(*Sec, TEXT("Menu_Background"), ConfigName);
	GConfig->Flush(false);
}

void FUE4EditorCustomizeModule::ResetTextStyle()
{
	FString ConfigName = GetMutableDefault<UEditorCustomizeSetting>()->GetClass()->GetConfigName();
	FString Sec = GetMutableDefault<UEditorCustomizeSetting>()->GetClass()->GetPathName();
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
	GConfig->RemoveKey(*Sec, TEXT("Log_Normal"), ConfigName);
	GConfig->Flush(false);
}

void FUE4EditorCustomizeModule::RestoreCachedBrush()
{
	FSlateStyleSet* EditorStyles = (FSlateStyleSet*)&FEditorStyle::Get();
	FSlateStyleSet* CoreStyles = (FSlateStyleSet*)&FCoreStyle::Get();
	for (auto it=CachedOriginalBrushes.CreateConstIterator();it;++it)
	{
		EditorStyles->Set(it->Key, (FSlateBrush*)it->Value);
	}

	for (auto it=CachedCustomBrushesEditor.CreateConstIterator();it;++it)
		EditorStyles->Set(it->Key, (FSlateBrush*)it->Value);
	for (auto it = CachedCustomBrushesCore.CreateConstIterator(); it; ++it)
		CoreStyles->Set(it->Key, (FSlateBrush*)it->Value);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUE4EditorCustomizeModule, UE4EditorCustomize)
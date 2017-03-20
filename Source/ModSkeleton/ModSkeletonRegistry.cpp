// Copyright 2017 Smogworks
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http ://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "ModSkeleton.h"
#include "ModSkeletonRegistry.h"

#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

#include "Runtime/PakFile/Public/IPlatformFilePak.h"

#include "AssetRegistryModule.h"

#include "ModSkeletonPluginInterface.h"

UModSkeletonRegistry::UModSkeletonRegistry()
{
	FModSkeletonHookDescription InitHook;
	InitHook.AlwaysInvoke = true;
	InitHook.HookName = "ModSkeletonInit";
	InitHook.HookDescription = "ModSkeleton Bootstrap Entrypoint. This will be invoked on every Mod as they are loaded.";

	InstallHook(InitHook);
}

void UModSkeletonRegistry::ScanForModPlugins()
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	// Uncomment this and the "PakPlatform->IterateDirectoryRecursively" below to dump out pak contents on load
	//struct StructDumpVisitor : public IPlatformFile::FDirectoryVisitor
	//{
	//	virtual bool Visit(const TCHAR* FilenameOrDirectory, bool bIsDirectory)
	//	{
	//		if (bIsDirectory)
	//		{
	//			UE_LOG(ModSkeletonLog, Log, TEXT(" - DumpVisitor Directory: %s"), FilenameOrDirectory);
	//		}
	//		else
	//		{
	//			UE_LOG(ModSkeletonLog, Log, TEXT(" - DumpVisitor File: %s"), FilenameOrDirectory);
	//		}
	//		return true;
	//	}
	//};
	//StructDumpVisitor DumpVisitor;
	
	IPlatformFile& InnerPlatform = FPlatformFileManager::Get().GetPlatformFile();
	FPakPlatformFile* PakPlatform = new FPakPlatformFile();
	PakPlatform->Initialize(&InnerPlatform, TEXT(""));
	FPlatformFileManager::Get().SetPlatformFile(*PakPlatform);

	IFileManager& FileManager = IFileManager::Get();
	FString PakPath = FPaths::GameContentDir() + TEXT("Paks");
	FPaths::NormalizeDirectoryName(PakPath);
	FString BinSearch = PakPath + "/*.bin";

	TArray<FString> Files;
	FileManager.FindFiles(Files, *BinSearch, true, false);
	UE_LOG(ModSkeletonLog, Log, TEXT("Searching for Pak AssetRegistries: %s"), *BinSearch);

	for (int32 i = 0; i < Files.Num(); ++i) {
		FString BinFilename = PakPath + TEXT("/") + Files[i];
		FPaths::MakeStandardFilename(BinFilename);
		UE_LOG(ModSkeletonLog, Log, TEXT(" - AssetRegistry: %s"), *BinFilename);

		FString PathPart;
		FString FilenamePart;
		FString ExtensionPart;
		FPaths::Split(BinFilename, PathPart, FilenamePart, ExtensionPart);
		FString PakFilename = PathPart + "/" + FilenamePart + ".pak";
		FPaths::MakeStandardFilename(PakFilename);

		if (LoadedPaks.Contains(PakFilename)) {
			continue;
		}

		if (FPaths::FileExists(PakFilename))
		{
			UE_LOG(ModSkeletonLog, Log, TEXT("Attempting PakLoad: %s"), *PakFilename);

			// TODO - Would prefer to use this, but I cannot seem to make the mount paths correct for it
			//if (!FCoreDelegates::OnMountPak.Execute(PakFilename, 0, &DumpVisitor))
			//{
			//	UE_LOG(ModSkeletonLog, Error, TEXT("Failed to mount pak file: %s"), *PakFilename);
			//	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Failed to mount pak file: %s"), *PakFilename));
			//	continue;
			//}

			FString MountPoint(FPaths::GetPath(PakFilename));

			FPakFile PakFile(&InnerPlatform, *PakFilename, false);
			if (!PakFile.IsValid())
			{
				UE_LOG(ModSkeletonLog, Error, TEXT("Invalid pak file: %s"), *PakFilename);
				GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Invalid pak file: %s"), *PakFilename));
				continue;
			}

			PakFile.SetMountPoint(*MountPoint);
			if (!PakPlatform->Mount(*PakFilename, 0, *MountPoint))
			{
				UE_LOG(ModSkeletonLog, Error, TEXT("Failed to mount pak file: %s"), *PakFilename);
				GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Failed to mount pak file: %s"), *PakFilename));
				continue;
			}

			LoadedPaks.Add(PakFilename, true);

			FString MountTarget = FPaths::Combine(*MountPoint, TEXT("Plugins"), *FilenamePart, TEXT("Content/"));
			UE_LOG(ModSkeletonLog, Log, TEXT(" - Mounting At: %s"), *MountTarget);
			FPackageName::RegisterMountPoint(TEXT("/") + FilenamePart + TEXT("/"), MountTarget);

			FArrayReader SerializedAssetData;
			if (FFileHelper::LoadFileToArray(SerializedAssetData, *BinFilename))
			{
				AssetRegistry.Serialize(SerializedAssetData);
				UE_LOG(ModSkeletonLog, Log, TEXT(" - AssetRegistry Loaded (%d bytes): %s"), SerializedAssetData.Num(), *BinFilename);
				//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Emerald, FString::Printf(TEXT(" - AssetRegistry Loaded (%d bytes): %s"), SerializedAssetData.Num(), *BinFilename));
			}

			//PakPlatform->IterateDirectoryRecursively(*MountTarget, DumpVisitor);
		}
	}
	
	TArray<FAssetData> AssetData;
	AssetRegistry.GetAllAssets(AssetData);

	UE_LOG(ModSkeletonLog, Log, TEXT("Searching for ModSkeleton Mod Assets:"));

	for (int32 i = 0; i < AssetData.Num(); ++i) {
		FString name = AssetData[i].AssetName.ToString();
		if (name.StartsWith("MOD_SKELETON", ESearchCase::CaseSensitive)) {
			UE_LOG(ModSkeletonLog, Log, TEXT(" - Asset: %s %s %s %s"), *name, *AssetData[i].PackagePath.ToString(), *AssetData[i].ObjectPath.ToString(), *AssetData[i].AssetClass.ToString());

			if (LoadedPlugins.Contains(AssetData[i].ObjectPath)) {
				continue;
			}

			UClass* AssetClass = LoadObject<UClass>(nullptr, *(TEXT("Class'") + AssetData[i].ObjectPath.ToString() + TEXT("_C'")));
			if (AssetClass != nullptr)
			{
				UObject *RealObj = NewObject<UObject>(this, AssetClass);
				if (RealObj->GetClass()->ImplementsInterface(UModSkeletonPluginInterface::StaticClass()))
				{
					TArray< UBPVariant* > HookIO;
					IModSkeletonPluginInterface::Execute_ModSkeletonHook(RealObj, TEXT("ModSkeletonInit"), HookIO);

					//FString PlugName;
					//TSubclassOf<UUserWidget> ModDescription;
					//IModSkeletonPluginInterface::Execute_ModSkeletonInitialize(RealObj, PlugName, ModDescription);

					//TArray< UBPVariant* > InParams;
					//TArray< UBPVariant* > OutResults;
					//IModSkeletonPluginInterface::Execute_ModSkeletonHook(RealObj, TEXT("ModSkeletonInit"), InParams, OutResults);

					//for (int32 i = 0; i < OutResults.Num(); ++i)
					//{
					//	UE_LOG(ModSkeletonLog, Warning, TEXT("%s"), *OutResults[i]->GetDebugValue());
					//}

					//UModSkeletonPluginRef* PlugRef = NewObject<UModSkeletonPluginRef>(this, UModSkeletonPluginRef::StaticClass());
					//PlugRef->ModName = PlugName;
					//PlugRef->ModObject = RealObj;
					//PlugRef->ModDescription = ModDescription;

					LoadedPlugins.Add(AssetData[i].ObjectPath, RealObj);
				}
			}
		}
	}
}

void UModSkeletonRegistry::ListModPlugins(TArray< UObject* >& OutPluginList)
{
	LoadedPlugins.GenerateValueArray(OutPluginList);
}

bool UModSkeletonRegistry::InstallHook(FModSkeletonHookDescription HookDescription)
{
	if (RegisteredHooks.Contains(HookDescription.HookName))
	{
		return false;
	}
	RegisteredHooks.Add(HookDescription.HookName, HookDescription);
	return true;
}

TArray< FModSkeletonHookDescription > UModSkeletonRegistry::ListHooks()
{
	TArray< FModSkeletonHookDescription > OutArray;
	RegisteredHooks.GenerateValueArray(OutArray);
	return OutArray;
}

void UModSkeletonRegistry::ConnectHook(FString HookName, int32 Priority, UObject *ModSkeletonPluginInterface)
{
	if (!ModSkeletonPluginInterface->GetClass()->ImplementsInterface(UModSkeletonPluginInterface::StaticClass())) {
		return;
	}

	FModSkeletonConnectHook NewHook;
	NewHook.HookName = HookName;
	NewHook.Priority = Priority;
	NewHook.ModSkeletonPluginInterface = ModSkeletonPluginInterface;

	ConnectedHooks.HeapPush(NewHook, FModSkeletonConnectHookPredicate());
}

TArray< UBPVariant* > UModSkeletonRegistry::InvokeHook(FString HookName, const TArray< UBPVariant * >& HookIO)
{
	if (!RegisteredHooks.Contains(HookName))
	{
		UE_LOG(ModSkeletonLog, Warning, TEXT("Ignoring Unregistered HookName: %s"), *HookName);
		return HookIO;
	}
	UE_LOG(ModSkeletonLog, Log, TEXT("Invoke HookName: %s"), *HookName);
	TArray< UBPVariant* > CurHookIO = HookIO;
	FModSkeletonHookDescription HookDescription = RegisteredHooks[HookName];
	if (HookDescription.AlwaysInvoke) {
		for (auto PlugRef : LoadedPlugins)
		{
			CurHookIO = IModSkeletonPluginInterface::Execute_ModSkeletonHook(PlugRef.Value, HookName, CurHookIO);
		}
	}
	else
	{
		TArray<FModSkeletonConnectHook> ConnectedHooksClone = ConnectedHooks;
		FModSkeletonConnectHook Next;
		while (ConnectedHooksClone.Num() > 0)
		{
			ConnectedHooksClone.HeapPop(Next, FModSkeletonConnectHookPredicate());
			if (Next.HookName != HookName) continue;
			CurHookIO = IModSkeletonPluginInterface::Execute_ModSkeletonHook(Next.ModSkeletonPluginInterface, HookName, CurHookIO);
		}
	}
	return CurHookIO;
}

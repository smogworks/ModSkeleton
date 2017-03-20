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

#pragma once

#include "UObject/NoExportTypes.h"
#include "ModSkeletonRegistry.generated.h"

/**
 * This struct describes the API of an individual hook
 */
USTRUCT(BlueprintType, Category = "ModSkeleton")
struct FModSkeletonHookDescription
{
	GENERATED_BODY()

	/**
	 * AlwaysInvoke Hooks to not participate in prioritization, and, when invoked,
	 * are called on every registered ModPlugin.
	 * "ModSkeletonInit" is an AlwaysInvoke Hook.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ModSkeletonHookDescription")
	bool AlwaysInvoke;

	/**
	 * Globally unique name of the hook
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ModSkeletonHookDescription")
	FString HookName;

	/**
	 * Description of what the hook intends to accomplish
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ModSkeletonHookDescription")
	FString HookDescription;

	/**
	 * Descriptions for each parameter/result in the HookIO array
	 * Recommend including type information, like:
	 *   [
	 *     "ButtonList {Array<UserWidget>} - Array of buttons to be added to the menu",
	 *     "LaunchMapping {Array<String>} - Input actions that cause this menu to be shown"
	 *   ]
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ModSkeletonHookDescription")
	TArray<FString> HookIODescription;
};

/**
 * This is an internal structure for tracking hook connections
 */
USTRUCT()
struct FModSkeletonConnectHook
{
	GENERATED_BODY()

	/**
	 * Connected HookName
	 */
	UPROPERTY()
	FString HookName;

	/**
	 * Priority - higher numbers are invoked first
	 */
	UPROPERTY()
	int32 Priority;

	/**
	 * Link to the UObject on which to invoke this hook.
	 * Must implement ModSkeletonPluginInterface
	 */
	UPROPERTY()
	UObject *ModSkeletonPluginInterface;
};

/**
 * Heap priority sort predicate helper
 */
struct FModSkeletonConnectHookPredicate
{
	bool operator()(const FModSkeletonConnectHook& A, const FModSkeletonConnectHook& B) const
	{
		// higher priorities to the top
		return A.Priority > B.Priority;
	}
};

/**
 * This object loads all mod packages, invokes any MOD_SKELETON ModSkeletonInit interfaces found
 * And keeps track of all registered mod hooks and connections.
 */
UCLASS(BlueprintType)
class MODSKELETON_API UModSkeletonRegistry : public UObject
{
	GENERATED_BODY()
	
public:
	UModSkeletonRegistry();

	/**
	 * Invoked automaticall by ModSkeletonGameInstance...
	 * Should be safe to invoke at runtime to load any new modules added to directory
	 * will not re-load any mods that have changed or un-load any deleted ones
	 */
	UFUNCTION(BlueprintCallable, Category = "ModSkeleton")
	virtual void ScanForModPlugins();

	/**
	 * Get a list of all loaded plugin init interfaces
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ModSkeleton")
	virtual void ListModPlugins(TArray< UObject* >& OutPluginList);

	/**
	 * Install a new hook to the mod system
	 */
	UFUNCTION(BlueprintCallable, Category = "ModSkeleton")
	virtual bool InstallHook(FModSkeletonHookDescription HookDescription);

	/**
	 * List all hooks that have been installed
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ModSkeleton")
	virtual TArray< FModSkeletonHookDescription > ListHooks();

	/**
	 * Get a HookDescription for a single hook
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ModSkeleton")
	virtual FModSkeletonHookDescription GetHookDescription(FString HookName);

	/**
	 * Connect a plugin interface to a HookName at a given priority
	 */
	UFUNCTION(BlueprintCallable, Category = "ModSkeleton")
	virtual void ConnectHook(FString HookName, int32 Priority, UObject *ModSkeletonPluginInterface);

	/**
	 * Invoke an installed hook
	 */
	UFUNCTION(BlueprintCallable, Category = "ModSkeleton")
	virtual TArray< UBPVariant* > InvokeHook(FString HookName, const TArray< UBPVariant* >& HookIO);

private:
	/**
	 * Keep track of loaded pak names so we don't re-load one we've loaded before
	 */
	UPROPERTY()
	TMap<FString, bool> LoadedPaks;

	/**
	 * Keep track of all initialized MOD_SKELETON init interfaces, so we don't re-init any
	 */
	UPROPERTY()
	TMap<FName, UObject *> LoadedPlugins;

	/**
	 * Keep track of all installed hook descriptions
	 */
	UPROPERTY()
	TMap<FString, FModSkeletonHookDescription> RegisteredHooks;

	/**
	 * USE AS A HEAP - always access with heap functions using FModSkeletonConnectHookPredicate
	 * The priority heap of connected hooks
	 */
	UPROPERTY()
	TArray<FModSkeletonConnectHook> ConnectedHooks;
};

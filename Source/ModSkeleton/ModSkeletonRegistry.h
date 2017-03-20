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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ModSkeletonHookDescription")
	FString HookName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ModSkeletonHookDescription")
	FString HookDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ModSkeletonHookDescription")
	TArray<FString> HookIODescription;
};

USTRUCT()
struct FModSkeletonConnectHook
{
	GENERATED_BODY()

	UPROPERTY()
	FString HookName;

	UPROPERTY()
	int32 Priority;

	UPROPERTY()
	UObject *ModSkeletonPluginInterface;
};

struct FModSkeletonConnectHookPredicate
{
	bool operator()(const FModSkeletonConnectHook& A, const FModSkeletonConnectHook& B) const
	{
		// higher priorities to the top
		return A.Priority > B.Priority;
	}
};

/**
 * 
 */
UCLASS(BlueprintType)
class MODSKELETON_API UModSkeletonRegistry : public UObject
{
	GENERATED_BODY()
	
public:
	UModSkeletonRegistry();

	UFUNCTION(BlueprintCallable, Category = "ModSkeleton")
	virtual void ScanForModPlugins();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ModSkeleton")
	virtual void ListModPlugins(TArray< UObject* >& OutPluginList);

	UFUNCTION(BlueprintCallable, Category = "ModSkeleton")
	virtual bool InstallHook(FModSkeletonHookDescription HookDescription);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ModSkeleton")
	virtual TArray< FModSkeletonHookDescription > ListHooks();

	UFUNCTION(BlueprintCallable, Category = "ModSkeleton")
	virtual void ConnectHook(FString HookName, int32 Priority, UObject *ModSkeletonPluginInterface);

	UFUNCTION(BlueprintCallable, Category = "ModSkeleton")
	virtual TArray< UBPVariant* > InvokeHook(FString HookName, const TArray< UBPVariant* >& HookIO);

private:
	UPROPERTY()
	TMap<FString, bool> LoadedPaks;

	UPROPERTY()
	TMap<FName, UObject *> LoadedPlugins;

	UPROPERTY()
	TMap<FString, FModSkeletonHookDescription> RegisteredHooks;

	UPROPERTY()
	TArray<FModSkeletonConnectHook> ConnectedHooks;
};

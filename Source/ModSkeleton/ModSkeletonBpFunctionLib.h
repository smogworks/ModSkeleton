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

#include "ModSkeletonRegistry.h"
#include "ModSkeletonGameInstance.h"

#include "Kismet/BlueprintFunctionLibrary.h"
#include "ModSkeletonBpFunctionLib.generated.h"

/**
 * Some blueprint helper functions to make working with ModSkeleton easier
 */
UCLASS()
class MODSKELETON_API UModSkeletonBpFunctionLib : public UBlueprintFunctionLibrary
{
	friend class UModSkeletonGameInstance;

	GENERATED_BODY()

public:

	/**
	 * Get access to the Registry singleton
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ModSkeleton")
	static UModSkeletonRegistry* ModSkeletonRegistryGet();

	/**
	 * Helper compact BP node to dump the hook description struct
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ModSkeletonHookDescription", meta = (CompactNodeTitle = "FullDescription"))
	static FString GetFullDescription(const FModSkeletonHookDescription& HookDescription);

private:
	static UModSkeletonRegistry* GlobalModRegistryRef;
};

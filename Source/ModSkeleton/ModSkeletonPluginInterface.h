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

#include "BPVariant.h"

#include "ModSkeletonPluginInterface.generated.h"

UINTERFACE(MinimalAPI)
class UModSkeletonPluginInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 * This Interface allows communication between mods that have not yet been written
 */
class MODSKELETON_API IModSkeletonPluginInterface
{
	GENERATED_IINTERFACE_BODY()

public:
	
	/**
	 * Any "Connected" Hook that is invoked will invoke this function if you implement it.
	 * If your uclass begins with the case-sensitive string "MOD_SKELETON" then "ModSkeletonInit" will also be invoked.
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ModSkeleton")
	TArray< UBPVariant*> ModSkeletonHook(const FString& HookName, const TArray< UBPVariant* >& HookIO);
};

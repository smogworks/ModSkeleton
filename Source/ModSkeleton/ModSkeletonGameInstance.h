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

#include "Kismet/BlueprintPlatformLibrary.h"
#include "ModSkeletonGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MODSKELETON_API UModSkeletonGameInstance : public UPlatformGameInstance
{
	GENERATED_BODY()
	
public:

	virtual void Init() override;
	virtual void Shutdown() override;
	
	UPROPERTY(BlueprintReadOnly, Category="ModSkeleton")
	UModSkeletonRegistry* ModRegistry;

};
